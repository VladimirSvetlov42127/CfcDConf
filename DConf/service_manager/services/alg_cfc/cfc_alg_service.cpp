#include "cfc_alg_service.h"

#include "service_manager/services/alg_cfc/cfc_alg_manager.h"
#include "db/dc_db_manager.h"
#include "file_managers/DcFlexLogicFileManager.h"
#include "data_model/dc_properties.h"

#include "gui/forms/algorithms/custom/Nodes/bi_node.h"
#include "gui/forms/algorithms/custom/Nodes/bo_node.h"

namespace {

constexpr const char* PROPERTY_DESCRIPTION = "DESC";
constexpr const char* PROPERTY_COMPILED = "COMPILED";

} // namespace

CfcAlgService::CfcAlgService(uint8_t id, CfcAlgManager *manager)
    : Service { QString("Гибкая логика %1").arg(id) }
    , m_manager{manager}
    , m_id{id}
    , m_parser{new DepCfcParser}
{
    m_ios.reserve(32);
}

CfcAlgService::~CfcAlgService()
{
    m_parser->deleteLater();
}

Service::Type CfcAlgService::type() const
{
    return Service::CfcAlgType;
}

uint8_t CfcAlgService::id() const
{
    return m_id;
}

bool CfcAlgService::isEnabled() const
{
    return !fileElement()->value().isEmpty();
}

void CfcAlgService::setEnabled(bool enabled)
{
    QString filePath = enabled ? deviceBcaFileName() : QString();
    fileElement()->updateValue(filePath);
}

bool CfcAlgService::isCompiled() const
{
    return !m_compiledData.isEmpty();
}

QByteArray CfcAlgService::compiledData() const
{
    return m_compiledData;
}

void CfcAlgService::setCompiledData(const QByteArray &data)
{    
    m_compiledData = data;
    dbUpdateProperties();
    auto localBcaFilePath = localBcaFileName();
    if (m_compiledData.isEmpty()) {
        QFile::remove(localBcaFilePath);
        setEnabled(false);
        return;
    }

    QFile file(localBcaFilePath);
    if (!file.open(QIODevice::WriteOnly))
        return;

    file.write(m_compiledData);
}

QString CfcAlgService::description() const
{
    return m_descriptrion;
}

void CfcAlgService::setDescription(const QString &desc)
{
    m_descriptrion = desc;
    dbUpdateProperties();
}

CfcServiceInput *CfcAlgService::makeInput()
{
    auto pin = freePin();
    if (!pin)
        return nullptr;

    auto ioID = m_manager->takeFreeIOID();
    if (!ioID)
        return nullptr;

    return makeInput(ioID.value(), pin.value());
}

CfcServiceOutput *CfcAlgService::makeOutput()
{
    auto pin = freePin();
    if (!pin)
        return nullptr;

    auto ioID = m_manager->takeFreeIOID();
    if (!ioID)
        return nullptr;

    return makeOutput(ioID.value(), pin.value());
}

void CfcAlgService::removeInput(CfcServiceInput *input)
{
    input->setSource(nullptr);
    dbRemove(input);    // Запись в таблицу привязки для совместимости

    m_manager->releaseIOID(input->id());
    auto &[bindElement, sio] = m_ios[input->pin()];
    sio = nullptr;
    Service::deleteInput(input);    
}

void CfcAlgService::removeOutput(CfcServiceOutput *output)
{
    output->setTarget(nullptr);
    dbRemove(output);   // Запись в таблицу привязки для совместимости

    m_manager->releaseIOID(output->id());
    auto &[bindElement, sio] = m_ios[output->pin()];
    sio = nullptr;    
    Service::deleteOutput(output);
}

DepCfcParser *CfcAlgService::parser() const
{
    return m_parser;
}

bool CfcAlgService::save()
{
    parser()->setTitle(name());
    return parser()->saveFile(localGraphFileName());
}

bool CfcAlgService::hasInvalidInput() const
{
    for(auto &input: inputs()) {
        auto vdin = input->source()->to<VirtualInputSignal>();
        if (vdin && !vdin->source())
            return true;
    }

    return false;
}

ParameterElement *CfcAlgService::bindElement(uint8_t pin) const
{
    if (pin < m_ios.size())
        return m_ios.at(pin).first;

    return nullptr;
}

ParameterElement *CfcAlgService::fileElement() const
{
    return m_fileElement;
}

void CfcAlgService::setFileElement(ParameterElement *fileElement)
{
    m_fileElement = fileElement;
}

std::optional<uint8_t> CfcAlgService::freePin() const
{
    for(size_t i = 0; i < m_ios.size(); ++i)
        if (!m_ios.at(i).second)
            return i;

    return {};
}

bool CfcAlgService::isFree(uint8_t pin) const
{
    if (pin < m_ios.size())
        return !m_ios.at(pin).second;

    return false;
}

CfcAlgService::UPtr CfcAlgService::create(uint8_t position, CfcAlgManager *manager)
{
    auto fileElement = manager->config()->paramsRegistry().element(SP_FILE_FLEXLGFILES, position);
    if (!fileElement)
        return nullptr;

    auto bindParameter = manager->config()->paramsRegistry().parameter(SP_FLEXLGCROSSTABLE);
    if (!bindParameter || position >= bindParameter->profiles().size())
        return nullptr;

    auto algID = position + 1;
    auto alg = CfcAlgService::UPtr(new CfcAlgService(algID, manager));
    alg->setFileElement(fileElement);
    auto& positionProfile = bindParameter->profiles().at(position);
    for(auto& bindElement: positionProfile)
        alg->m_ios.emplace_back(bindElement.get(), nullptr);

    return alg;
}

CfcAlgService::UPtr CfcAlgService::load(uint8_t position, CfcAlgManager *manager)
{
    auto algID = position + 1;
    auto graphFilePath = DcFlexLogicFileManager(manager->config()).localGraphFileName(algID);
    if (!QFile::exists(graphFilePath))
        return nullptr;

    auto alg = create(position, manager);
    if (!alg)
        return nullptr;

    if (!alg->parser()->loadFile(graphFilePath))
        return nullptr;

    auto bcaFilePath = DcFlexLogicFileManager(manager->config()).localBcaFileName(algID);
    QFile bcaFile(bcaFilePath);
    if (bcaFile.open(QIODevice::ReadOnly))
        alg->m_compiledData = bcaFile.readAll();

    alg->setName(alg->parser()->title());
    auto nodes = alg->parser()->editorNodes();
    for (auto node: nodes) {
        if (node->nodeType() != FlexLogic::RZA_LOAD)
            continue;

        // Параметры входа/выхода
        // В файле alg_pin = ножке входа/выхода алгоритма. Начинается с 1. меньше или равно 0 => Невалидно.
        int io_pin = node->param("alg_pin").value.toInt();
        if (io_pin <= 0)
            continue;

        // В системе параметров все io_pin начинаются с 0. io_pin должнет быть сводобным.
        io_pin -= 1;
        if (!alg->isFree(io_pin))
            continue;

        // Элемент параметра соответсвующий io_pin.
        auto bindElement = alg->bindElement(io_pin);
        if (!bindElement)
            continue;

        // В файле io_id - глоблаьный id входа/выхода. Начинается с 1. меньше или равно 0 => Невалидно.
        // io_id должен быть свободным.
        int io_id = node->param("io_id").value.toInt();
        if (io_id <= 0 || !manager->takeIOID(io_id))
            continue;

        uint16_t bindValue = bindElement->value().toUInt();
        if (node->name() == "BI") {
            auto input = alg->makeInput(io_id, io_pin);
            input->setSource(manager->config()->serviceManager()->din(bindValue));
            static_cast<BI*>(node)->setCfcInput(input);
        }

        if (node->name() == "BO") {
            auto output = alg->makeOutput(io_id, io_pin);
            output->setTarget(manager->config()->serviceManager()->vdin(bindValue));
            static_cast<BO*>(node)->setCfcOutput(output);
        }
    }

    return alg;
}

CfcServiceInput *CfcAlgService::makeInput(uint16_t ioID, uint8_t pin)
{
    if (!isFree(pin))
        return nullptr;

    auto &[bindElement, sio] = m_ios[pin];
    auto inputUPtr = std::make_unique<CfcServiceInput>(ioID, pin, bindElement, this);
    auto input = inputUPtr.get();
    sio = input;
    Service::addInput(std::move(inputUPtr));

    // Запись в таблицу привязки для совместимости
    dbInsert(input);
    return input;
}

CfcServiceOutput *CfcAlgService::makeOutput(uint16_t ioID, uint8_t pin)
{
    if (!isFree(pin))
        return nullptr;

    auto &[bindElement, sio] = m_ios[pin];
    auto outputUPtr = std::make_unique<CfcServiceOutput>(ioID, pin, bindElement, this);
    auto output = outputUPtr.get();
    sio = output;
    Service::addOutput(std::move(outputUPtr));

    // Запись в таблицу привязки для совместимости
    dbInsert(output);
    return output;
}

void CfcAlgService::dbInsert(ServiceIO *io)
{
    auto uid = fileElement()->uid();
    if (!uid)
        return;

    auto query = QString("INSERT INTO alg_cfc_io(cfc_io_id, cfc_alg_id, cfc_alg_pin, cfc_io_direction, name) VALUES(%1, %2, %3, %4, '%5');")
            .arg(io->id())
            .arg(id())
            .arg(io->pin())
            .arg(1)
            .arg(io->name());
    gDbManager.execute(uid, query);
}

void CfcAlgService::dbRemove(ServiceIO *io)
{
    auto uid = fileElement()->uid();
    if (!uid)
        return;

    auto query = QString("DELETE FROM alg_cfc_io WHERE cfc_io_id = %1;").arg(io->id());
    gDbManager.execute(uid, query);
}

void CfcAlgService::dbUpdateProperties()
{
    auto uid = fileElement()->uid();
    if (!uid)
        return;

    auto query = QString("UPDATE algs_cfc SET name = '%1', properties = '%2' WHERE cfc_alg_id = %3;")
            .arg(name(), properties())
            .arg(id());
    gDbManager.execute(uid, query);
}

QString CfcAlgService::localGraphFileName() const
{
    return DcFlexLogicFileManager(m_manager->config()).localGraphFileName(id());
}

QString CfcAlgService::localImageFileName() const
{
    return DcFlexLogicFileManager(m_manager->config()).localImageFileName(id());
}

QString CfcAlgService::localBcaFileName() const
{
    return DcFlexLogicFileManager(m_manager->config()).localBcaFileName(id());
}

QString CfcAlgService::deviceBcaFileName() const
{
    return DcFlexLogicFileManager(m_manager->config()).deviceBcaFileName(id());
}

void CfcAlgService::clear()
{
    while(inputs().size()) {
        auto input = static_cast<CfcServiceInput*>(inputs().back().get());
        removeInput(input);
    }

    while(outputs().size()) {
        auto output = static_cast<CfcServiceOutput*>(outputs().back().get());
        removeOutput(output);
    }

    QFile::remove(localBcaFileName());
    QFile::remove(localGraphFileName());
    QFile::remove(localImageFileName());
}

QString CfcAlgService::properties() const
{
    DcProperties prop;
    prop.set(PROPERTY_COMPILED, QString::number(isCompiled()));
    prop.set(PROPERTY_DESCRIPTION, description());
    return prop.toJson();
}
