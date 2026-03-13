#include "dc_db_manager.h"

#include <qdebug.h>

#include <dpc/sybus/ParamAttribute.h>
#include "data_model/parameters/parameter_element.h"
#include "data_model/parameters/parameter_element_board.h"

#define TRACE(msg) \
    qDebug().noquote() << QString("[DB] %1").arg(msg);

namespace {

const char* SELECT_SETTINGS             = "SELECT * FROM settings";
const char* SELECT_CFG_HEADLINE         = "SELECT * FROM cfg_parameters_headline";
const char* SELECT_CFG_ITEMS            = "SELECT * FROM cfg_parameters_items ORDER BY address, param_index";
const char* SELECT_BOARDS               = "SELECT * FROM boards";
const char* SELECT_BOARD_CFG_PARAMETERS = "SELECT * FROM board_cfg_parameters ORDER BY addr, param";
const char* SELECT_SIGNALS              = "SELECT * FROM signals";
const char* SELECT_ALGS                 = "SELECT * FROM algs ORDER BY name";
const char* SELECT_ALG_IO               = "SELECT * FROM alg_io";
const char* SELECT_ALGS_CFC             = "SELECT * FROM algs_cfc";
const char* SELECT_ALG_CFC_IO           = "SELECT * FROM alg_cfc_io";
const char* SELECT_MATRIX_ALG           = "SELECT * FROM matrix_alg";
const char* SELECT_MATRIX_ALG_CFC       = "SELECT * FROM matrix_alg_cfc";
const char* SELECT_MATRIX_SIGNALS       = "SELECT * FROM matrix_signals";

int callback_settings_controller(void *data, int argc, char **argv, char **) {

    if (data == nullptr)
        return 0;

    if (argc < 2)
        return 0;

    if ((argv[0] == nullptr) || (argv[1] == nullptr))
        return 0;

    QString setting_name = argv[0];
    QString setting_value = argv[1];

    auto pcontroller = static_cast<DcController*>(data);
    pcontroller->setSetting(setting_name, setting_value);

    return 0;
}

int callback_cfg_parameters_headline(void *data, int colomns_count, char **colonm_data_pointer, char **) {

    if (data == nullptr)
        return 0;

    if (colomns_count < 7) {
        //TODO to log transaction error
        return 0;
    }

    if ((colonm_data_pointer[0] == nullptr) || (colonm_data_pointer[1] == nullptr)) {
        //TODO to log error
        return 0;
    }

    int colomnInd = 0;	// Переменная для инкрементирования позиции в буфере
    QString name = colonm_data_pointer[colomnInd++];
    int dtype = atoi(colonm_data_pointer[colomnInd++]);
    int addr = atoi(colonm_data_pointer[colomnInd++]);
    QString flagstr;
    if (colonm_data_pointer[colomnInd] != nullptr)
        flagstr = colonm_data_pointer[colomnInd++];
    Dpc::Sybus::ParamAttribute flags(flagstr);
    uint32_t profiles_count = atoi(colonm_data_pointer[colomnInd++]);
    uint32_t subProfiles_count = atoi(colonm_data_pointer[colomnInd++]);
    uint16_t size_in_bytes = atoi(colonm_data_pointer[colomnInd++]);

    auto pcontroller = static_cast<DcController*>(data);
    auto parameter = std::make_unique<Parameter>(name, dtype, addr, flags, profiles_count, subProfiles_count, size_in_bytes);
    pcontroller->paramsRegistry().add(std::move(parameter));

    return 0;
}

int callback_cfg_parameters_items(void *data, int colomns_count, char **colomn_data_pointer, char **) {

    if (data == nullptr)
        return 0;

    if (colomns_count < 4) {
        //TODO to log transaction error
        return 0;
    }

    if ((colomn_data_pointer[0] == nullptr) || (colomn_data_pointer[1] == nullptr)) {
        //TODO to log error
        return 0;
    }

    int colomnInd = 0;	// Переменная для инкрементирования позиции в буфере
    QString name = colomn_data_pointer[colomnInd++];
    uint16_t addr = atoi(colomn_data_pointer[colomnInd++]);
    uint16_t ind = atoi(colomn_data_pointer[colomnInd++]);
    QString val = colomn_data_pointer[colomnInd++];

    auto pcontroller = static_cast<DcController*>(data);
    auto parameter = pcontroller->paramsRegistry().parameter(addr);
    if (!parameter)
        return 0;

    auto element = parameter->element(ind);
    if (element) {
        element->setName(name);
        element->updateValue(val);
    }

    return 0;
}

int callback_boards(void *data, int argc, char **argv, char **) {

    if (data == nullptr) {
        //TODO to log transaction error
        return 0;
    }

    if (argc < 4) {
        //TODO to log transaction error
        return 0;
    }

    if ((argv[0] == nullptr) || (argv[1] == nullptr) || (argv[2] == nullptr)) {
        //TODO to log error
        return 0;
    }

    int boardid = atoi(argv[0]);
    int slot = atoi(argv[1]);
    QString type = argv[2];
    int inst = 0;
    if (argv[3] != nullptr) {
        inst = atoi(argv[3]);
    }

    QString properties;
    if (argc > 4 && argv[4] != nullptr) {
        properties = argv[4];
    }

    auto pcontroller = static_cast<DcController*>(data);

    auto board = std::make_unique<DcBoard>(boardid, slot, type, inst, properties);
    pcontroller->append(std::move(board));

    return 0;
}

int callback_board_cfg_parameters(void *data, int argc, char **argv, char **) {

    if (data == nullptr) {
        //TODO to log transaction error
        return 0;
    }

    if (argc < 8) {
        //TODO to log transaction error
        return 0;
    }

    if ((argv[0] == nullptr) || (argv[1] == nullptr) || (argv[2] == nullptr)) {
        //TODO to log error
        return 0;
    }

    int boardid = atoi(argv[0]);
    int addr = atoi(argv[1]);
    int param = atoi(argv[2]);
    QString name = argv[3];
    int dtype = atoi(argv[4]);
    QString flagstr;
    if (argv[5] != nullptr)
        flagstr = argv[5];
    Dpc::Sybus::ParamAttribute flags(flagstr);
    QString value = argv[6];
    QString properties = argv[7];

    auto pcontroller = static_cast<DcController*>(data);
    auto pboard = pcontroller->board(boardid);
    if (!pboard) {
        // !!! В файле базы данных остаются записи, которые не используются.
        // !!! Потенциально возможна ситуация при которой не получится вставить новую запись из-за наличия записи с таким же 'primary key'
        return 0;
    }

    auto parameter = pboard->paramsRegistry().parameter(addr);
    if (!parameter) {
        parameter = pboard->paramsRegistry().add(std::make_unique<Parameter>(dtype, addr, flags));
    }

    parameter->append(std::make_unique<ParameterElementBoard>(boardid, param, name, value, properties, parameter));

    return 0;
}

int callback_signals_controller(void *data, int argc, char **argv, char **) {

    if (data == nullptr)
        return 0;

    if (argc < 7)
        return 0;

    if ((argv[0] == nullptr) || (argv[1] == nullptr) || (argv[2] == nullptr) || (argv[4] == nullptr) || (argv[5] == nullptr))
        return 0;

    int signalid = atoi(argv[0]);
    int internalid = atoi(argv[1]);
    auto direction = static_cast<DefSignalDirection>(atoi(argv[2]));
    auto type = static_cast<DefSignalType>(atoi(argv[3]));
    auto subtype = static_cast<DefSignalSubType>(atoi(argv[4]));
    QString name = argv[5];
    QString properties = argv[6];

    auto pcontroller = static_cast<DcController*>(data);
    auto psignal = new DcSignal(signalid, internalid, direction, type, subtype, name, properties, pcontroller);
    if (!pcontroller->addSignal(psignal, true)) {
        delete psignal;
    }

    return 0;
}

int callback_algs_controller(void *data, int argc, char **argv, char **) {

    if (data == nullptr)
        return 0;

    if (argc < 4)
        return 0;

    if ((argv[0] == nullptr) || (argv[1] == nullptr))
        return 0;

    int algid = atoi(argv[0]);
    int position = atoi(argv[1]);
    QString name = argv[2];
    QString properties = argv[3];

    auto pcontroller = static_cast<DcController*>(data);
    DcAlgInternal *palg = new DcAlgInternal(algid, position, name, properties, pcontroller);
    if (!pcontroller->algs_internal()->add(palg, true)) {
        delete palg;
    }

    return 0;
}

int callback_alg_io_controller(void *data, int argc, char **argv, char **) {

    if (data == nullptr)
        return 0;

    if (argc < 5)
        return 0;
    if ((argv[0] == nullptr) || (argv[1] == nullptr) || (argv[2] == nullptr) || (argv[3] == nullptr))
        return 0;

    int ioid = atoi(argv[0]);
    int algid = atoi(argv[1]);
    int algpin = atoi(argv[2]);
    DefIoDirection direction = (DefIoDirection)atoi(argv[3]);
    QString name = argv[4];

    auto pcontroller = static_cast<DcController*>(data);
    DcAlgInternal *palg = pcontroller->algs_internal()->getById(algid);
    if (palg == nullptr) {
        return 0;
    }

    DcAlgIOInternal *algio = new DcAlgIOInternal(ioid, algid, algpin, direction, name, pcontroller);
    if (!palg->ios()->add(algio, true)) {
        delete algio;
    }

    return 0;
}

int callback_algs_cfc_controller(void *data, int argc, char **argv, char **) {

    if (data == nullptr)
        return 0;

    if (argc < 4)
        return 0;

    if ((argv[0] == nullptr) || (argv[1] == nullptr) || (argv[2] == nullptr))
        return 0;

    int algid = atoi(argv[0]);
    int position = atoi(argv[1]);
    QString name = argv[2];
    QString properties = argv[3];

    auto pcontroller = static_cast<DcController*>(data);
    DcAlgCfc *palg = new DcAlgCfc(algid, position, name, properties, pcontroller);
    if (!pcontroller->algs_cfc()->add(palg, true)) {
        delete palg;
    }

    return 0;
}

int callback_alg_cfc_io_controller(void *data, int argc, char **argv, char **) {
    if (data == nullptr)
        return 0;

    if (argc < 5)
        return 0;

    if ((argv[0] == nullptr) || (argv[1] == nullptr) || (argv[2] == nullptr) || (argv[3] == nullptr))
        return 0;

    int ioid = atoi(argv[0]);
    int algid = atoi(argv[1]);
    int algpin = atoi(argv[2]);
    DefIoDirection direction = (DefIoDirection)atoi(argv[3]);
    QString name = argv[4];

    auto pcontroller = static_cast<DcController*>(data);
    DcAlgCfc *palg = pcontroller->algs_cfc()->getById(algid);
    if (palg == nullptr)
        return 0;

    DcAlgIOCfc *algio = new DcAlgIOCfc(ioid, algid, algpin, direction, name, pcontroller);
    if (!palg->ios()->add(algio, true)) {
        delete algio;
    }

    return 0;
}

int callback_matrix_alg_controller(void *data, int argc, char **argv, char **) {

    if (data == nullptr)
        return 0;

    if (argc < 3)
        return 0;

    if ((argv[0] == nullptr) || (argv[1] == nullptr))
        return 0;

    int srcid = atoi(argv[0]);
    int dstid = atoi(argv[1]);
    QString properties = argv[2];

    auto pcontroller = static_cast<DcController*>(data);
    DcMatrixElementAlg *pelem = new DcMatrixElementAlg(srcid, dstid, properties, pcontroller);
    if (!pcontroller->matrix_alg()->add(pelem, true)) {
        delete pelem;
    }

    return 0;
}

int callback_matrix_alg_cfc_controller(void *data, int argc, char **argv, char **) {

    if (data == nullptr)
        return 0;

    if (argc < 3)
        return 0;

    if ((argv[0] == nullptr) || (argv[1] == nullptr))
        return 0;

    int srcid = atoi(argv[0]);
    int dstid = atoi(argv[1]);
    QString properties = argv[2];

    auto pcontroller = static_cast<DcController*>(data);
    DcMatrixElementAlgCfc *pelem = new DcMatrixElementAlgCfc(srcid, dstid, properties, pcontroller);
    if (!pcontroller->matrix_cfc()->add(pelem, true)) {
        delete pelem;
    }

    return 0;
}

int callback_matrix_signals_controller(void *data, int argc, char **argv, char **) {

    if (data == nullptr)
        return 0;

    if (argc < 3)
        return 0;

    if ((argv[0] == nullptr) || (argv[1] == nullptr))
        return 0;

    int srcid = atoi(argv[0]);
    int dstid = atoi(argv[1]);
    QString properties = argv[2];

    auto pcontroller = static_cast<DcController*>(data);
    DcMatrixElementSignal *pelem = new DcMatrixElementSignal(srcid, dstid, properties, pcontroller);
    if (!pcontroller->matrix_signals()->add(pelem, true)) {
        delete pelem;
    }

    return 0;
}

void workerThread()
{
    while (gDbManager.isActive()) {
        if (gDbManager.testEvent())
            gDbManager.stepTransaction();
    }
}

} // namespace

DcDbManager& DcDbManager::getInstance()
{
    static DcDbManager    instance;
    return instance;
}

struct DcDbManager::StorageItem
{
    ~StorageItem()
    {
        if (db)
            sqlite3_close(db);
    }

    sqlite3* db = nullptr;
    DcController *device = nullptr;
    QString filePath;
};

DcController::UPtr DcDbManager::load(const QString &filePath, const QString &name)
{
    if (filePath.isEmpty())
        return nullptr;

    auto item = std::make_unique<StorageItem>();
    item->filePath = filePath;
    int rc = sqlite3_open_v2(filePath.toStdString().c_str(), &item->db, SQLITE_OPEN_READWRITE, nullptr);
    if (rc != SQLITE_OK) {
        makeError(sqlite3_errmsg(item->db), item.get());
        return nullptr;
    }

    auto device = std::make_unique<DcController>(filePath, name);
    item->device = device.get();

    if (!exec(item.get(), SELECT_SETTINGS, callback_settings_controller, device.get()))
        return nullptr;
    if (!exec(item.get(), SELECT_CFG_HEADLINE, callback_cfg_parameters_headline, device.get()))
        return nullptr;
    if (!exec(item.get(), SELECT_CFG_ITEMS, callback_cfg_parameters_items, device.get()))
        return nullptr;
    if (!exec(item.get(), SELECT_BOARDS, callback_boards, device.get()))
        return nullptr;
    if (!exec(item.get(), SELECT_BOARD_CFG_PARAMETERS, callback_board_cfg_parameters, device.get()))
        return nullptr;
    if (!exec(item.get(), SELECT_SIGNALS, callback_signals_controller, device.get()))
        return nullptr;
    if (!exec(item.get(), SELECT_ALGS, callback_algs_controller, device.get()))
        return nullptr;
    if (!exec(item.get(), SELECT_ALG_IO, callback_alg_io_controller, device.get()))
        return nullptr;
    if (!exec(item.get(), SELECT_ALGS_CFC, callback_algs_cfc_controller, device.get()))
        return nullptr;
    if (!exec(item.get(), SELECT_ALG_CFC_IO, callback_alg_cfc_io_controller, device.get()))
        return nullptr;
    if (!exec(item.get(), SELECT_MATRIX_ALG, callback_matrix_alg_controller, device.get()))
        return nullptr;
    if (!exec(item.get(), SELECT_MATRIX_ALG_CFC, callback_matrix_alg_cfc_controller, device.get()))
        return nullptr;
    if (!exec(item.get(), SELECT_MATRIX_SIGNALS, callback_matrix_signals_controller, device.get()))
        return nullptr;

    device->setUid(freeUid());
//    TRACE(QString("Loaded uid(%1), name(%2), file(%3)").arg(device->uid()).arg(device->name(), device->path()));
    m_connections.emplace(device->uid(), std::move(item));
    return device;
}

bool DcDbManager::execute(int32_t uid, const QString &transaction)
{
    if (transaction.isEmpty())
		return false;

	{
		std::unique_lock ul(m_queueMutex);
		m_transactions.push(std::make_pair(uid, transaction));
		m_queueCheck.notify_one();
    }
	
	return true;
}

bool DcDbManager::beginTransaction(int32_t uid)
{
    return execute(uid, "BEGIN TRANSACTION");
}

bool DcDbManager::endTransaction(int32_t uid)
{
    return execute(uid, "END TRANSACTION");
}

void DcDbManager::close(int32_t uid)
{
    auto it = m_connections.find(uid);
    if (it == m_connections.end())
        return;

//    TRACE(QString("Close uid(%1), name(%2), file(%3)").arg(uid).arg(it->second->device->name(), it->second->filePath));
    m_connections.erase(it);
}

void DcDbManager::closeAll()
{
    while(m_connections.size())
        close(m_connections.begin()->first);
}

bool DcDbManager::isActive() const
{
    return m_isActive;
}

bool DcDbManager::testEvent()
{
	std::unique_lock ul(m_queueMutex);
	if (m_transactions.empty()) {
		m_queueCheck.wait(ul, [this]() { return !m_transactions.empty() || !isActive();	});
		if (!isActive()) {
			return false;
		}
	}

	return true;
}

bool DcDbManager::stepTransaction()
{
	std::pair<int32_t, QString> ppair;

	{
        std::unique_lock lock(m_queueMutex);
        if (m_transactions.empty())
            return false;

		ppair = m_transactions.front();
		m_transactions.pop();        
	}

    auto item = getStorage(ppair.first);
    if (!item || !item->db) {
        makeError(QString("Не найдено соединенеие с db (id = %1): \"%2\"").arg(ppair.first).arg(ppair.second), item);
        return false;
    }

    int rc = sqlite3_exec(item->db, ppair.second.toStdString().c_str(), 0, 0, 0);
    if (rc != SQLITE_OK) {
        makeError(QString("%1: \"%2\"").arg(sqlite3_errmsg(item->db), ppair.second), item);
		return false;
	}
    return true;
}

DcDbManager::DcDbManager()
    : m_isActive(true)
{
    m_workerThread = std::thread(workerThread);
    m_connections.clear();
}

DcDbManager::~DcDbManager()
{
    m_isActive = false;
    m_queueCheck.notify_one();
    m_workerThread.join();
    closeAll();
}

bool DcDbManager::exec(StorageItem *item, const char *sql, int(*callback)(void*, int, char**, char**), void *data)
{
    if (!item->db) {
        makeError(QString("Не найдено соединенеие с db: \"%1\"").arg(sql), item);
        return false;
    }

    int rc = sqlite3_exec(item->db, sql, callback, data, 0);
    if (rc != SQLITE_OK) {
        makeError(QString("%1: \"%2\"").arg(sqlite3_errmsg(item->db), sql), item);
        return false;
    }

    return true;
}

DcDbManager::StorageItem* DcDbManager::getStorage(int32_t uid) const
{
    auto it = m_connections.find(uid);
    if (it == m_connections.end())
        return nullptr;

    return it->second.get();
}

void DcDbManager::makeError(const QString &errMsg, StorageItem *item)
{
    emit error(errMsg, item->filePath, item->device);
}

int32_t DcDbManager::freeUid() const
{
    int32_t uid = 0;
    while (getStorage(++uid))
        ;
    return uid;
}
