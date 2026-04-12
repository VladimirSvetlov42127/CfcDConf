#include "report_operation.h"

#include <set>

#include <QDesktopServices>
#include <QUrl>
#include <QDir>

#include <dpc/sybus/channel/Channel.h>
#include <dpc/sybus/utils.h>
#include <dpc/dpc_global.h>
#include <dpc/gui/widgets/journal/JournalWidget.h>
#include <dpc/gui/dialogs/msg_box/MsgBox.h>

#include "device_operations/report_operation/excel_zip_exporter.h"
#include "data_model/parameters/parameter.h"
#include "file_managers/IFileManager.h"

using namespace Dpc::Sybus;
using namespace Dpc::Gui;

namespace {

enum OperationSteps {
    ReadParamStep = 0,
    ReadFilesStep,

    TotalStepsCount
};
}

ReportOperation::ReportOperation(const QString &dirPath, QObject *parent)
    : AbstractOperation("Получение детального отчёта", TotalStepsCount, parent)
    , m_dirPath(dirPath)
{

}

void ReportOperation::onChannelProgress(int state)
{
    double totalProgressPerFile = 100.0 / m_filesCount;
    emitProgress(totalProgressPerFile * m_currentFile + totalProgressPerFile * state / 100.0);
}

bool ReportOperation::before()
{
    if (!clearDirectory())
        return false;

    return true;
}

bool ReportOperation::exec()
{
    if (!readParams())
        return false;

    if (!dowloadFiles())
        return false;

    return true;
}

bool ReportOperation::after()
{
    addInfo("Создание Excel файла...");

    ExcelZipExporter doc;
    if (!doc.convertToExcel(m_parameterContainer, m_dirPath)) {
        MsgBox::error("Ошибка при создании Excel файла!");
        return false;
    }
    addInfo("Excel файл создан...");

    QDesktopServices::openUrl(QUrl(m_dirPath));
    return true;
}

bool ReportOperation::readParams()
{
    m_parameterContainer.clear();

    // Адрес параметра "Список используемых параметров"
    uint16_t paramListAddr = SP_PARLIST;

    // Размерность параметра "Список используемых параметров"
    auto paramListDim = channel()->dimension(paramListAddr);
    if (!paramListDim) {
        addError(QString("Не удалось получить размерность параметра %1: %2").arg(toHex(paramListAddr), channel()->errorMsg()));
        return false;
    }

    // Пакет адреса используемых параметров
    auto paramListPack = channel()->param(paramListAddr, 0, paramListDim.subProfileCount());
    if (Channel::NoError != channel()->errorType()) {
        addError(QString("Не удалось получить значения параметра %1: %2").arg(toHex(paramListAddr), channel()->errorMsg()));
        return false;
    }

    // Список параметров по порядку из addrsListParamsUsed
    QList<uint16_t> orginalOrderAddrs;
    // Используется для сортировки адресов
    std::set<uint16_t> sortedAddrs;
    for (size_t i = 0; i < paramListPack->count(); ++i) {
        auto addr = paramListPack->value<uint16_t>(i);
        if(addr == SP_PARATTR || addr == SP_PARNAME || addr == SP_PARTYPE
            || addr == SP_PARDIM || addr == SP_PARSIZE || addr == SP_PARNAME_PERSONAL
            || addr == 0x59E1) {
            continue;
        }

        sortedAddrs.insert(addr);
        orginalOrderAddrs << addr;
    }

    // Список всех диапозонов адресов
    QList< QList<uint16_t> > addrListRanges;
    // Список текущего диапозона адресов
    QList<uint16_t> curRange;
    for (auto it = sortedAddrs.begin(); it != sortedAddrs.end(); ++it) {
        if (curRange.isEmpty() || curRange.last() == *it - 1) {
            curRange.append(*it);
            continue;
        }

        if (!curRange.isEmpty()) {
            addrListRanges.append(curRange);
        }

        curRange.clear();
        curRange.append(*it);
    }

    if (!curRange.empty())
        addrListRanges.push_back(curRange);


    // Таблица параметров: Адрес параметра -> объект параметра
    std::unordered_map<uint16_t, Parameter::UPtr> paramsMap;
    setCurrentStep(ReadParamStep, addrListRanges.size());
    int progress = 0;
    for(const auto &range : std::as_const(addrListRanges)) {
        emitProgress(++progress);

        // Пакет с типами каждого параметра находящегося в данном диапазоне
        auto typeRangePack = channel()->param(SP_PARTYPE, range.first(), range.size());
        if (Channel::NoError != channel()->errorType()) {
            addError(QString("Не удалось получить тип параметра : %1").arg(channel()->errorMsg()));
            return false;
        }

        // Пакет аттрибутов каждого параметра находящегося в данном диапазоне
        auto attrRangePack = channel()->param(SP_PARATTR, range.first(), range.size());
        if(Channel::NoError != channel()->errorType()) {
            addError(QString("Не удалось получить аттрибуты параметра : %1").arg(channel()->errorMsg()));
            return false;
        }

        // Пакет размерностей каждого параметра находящегося в данном диапазоне
        auto dimRangePack = channel()->param(SP_PARDIM, range.first(), range.size());
        if (!dimRangePack) {
            if (Channel::DeviceErrorType == channel()->errorType())
                continue;
            addError(QString("Не удалось получить размерность параметра : %1").arg(channel()->errorMsg()));
            return false;
        }

        // Пакет с длинной в байтах каждого параметра находящегося в данном диапазоне
        auto dataSizeRangePack = channel()->param(SP_PARSIZE, range.first(), range.size());
        if (Channel::NoError != channel()->errorType()) {
            addError(QString("Не удалось получить размер данных параметра : %1").arg(channel()->errorMsg()));
            return false;
        }

        for (int i = 0; i < range.size(); ++i) {
            // Тип параметра с индексом i находящегося в данном диапазоне
            uint8_t type = typeRangePack->value<uint8_t>(i);
            // Аттрибуты параметра с индексом i находящегося в данном диапазоне
            ParamAttribute attr(attrRangePack->value<uint16_t>(i));
            // Размерность параметра с индексом i находящегося в данном диапазоне
            ParamDimension dimension(dimRangePack->value<uint16_t>(i), attr);
            // Длина в байтах параметра с индексом i находящегося в данном диапазоне
            uint16_t dataSize = dataSizeRangePack->value<uint16_t>(i);

            // Параметр
            Parameter::UPtr param = std::make_unique<Parameter>(QString(), type, range[i], attr, dimension.profileCount(), dimension.subProfileCount(), dataSize);

            // Пакет всех имен параметра
            const auto &namesPack = channel()->names(range[i]);
            if (Channel::NoError != channel()->errorType()) {
                addError(QString("Не удалось получить имена параметра %1: %2").arg(toHex(range[i]), channel()->errorMsg()));
                return false;
            }

            // Имя параметра
            QString paramName = namesPack->count() ? namesPack->value<QString>() : QString();
            param->setName(paramName);

            // Индекс по которому заполняются имена и значения элементов параметра
            uint16_t idxName = 0;
            for (auto &p : param->profiles()) {
                // Пакет значений элементов параметра
                ParamPackPtr valuePack;
                if (range[i] != 0x6330 && attr[ParamAttribute::R]) {
                    valuePack = channel()->param(param->addr(), p.front()->position(), p.size());
                }

                for(size_t k = 0; k < p.size(); ++k) {
                    auto element = p.at(k).get();
                    element->setName(namesPack->value<QString>(++idxName, paramName));

                    if (valuePack && k < valuePack->count())
                        element->updateValue(valuePack->value<QString>(k));
                }
            }

            paramsMap.emplace(param->addr(), std::move(param));
        }
    }

    m_parameterContainer.reserve(paramsMap.size());
    for (const auto &addr : std::as_const(orginalOrderAddrs)) {
        auto it = paramsMap.find(addr);
        if (it != paramsMap.end()) {
            m_parameterContainer.push_back(std::move(it->second));
        }
    }

    return true;
}

bool ReportOperation::dowloadFiles()
{
    FileSystemLoginScope fsls(channel());
    if (!fsls.isValid) {
        addError(QString("Не удалось авторизоваться в файловую систему на устройстве: %1").arg(channel()->errorMsg()));
        return false;
    }

    QDir dirPath(m_dirPath);
    std::map<QString, QString> filesToDownload;
    for (auto &fm: fileManagersList()) {

        const auto deviceDirPath = fm->devicePath();
        const auto deviceDirName = QFileInfo(deviceDirPath).fileName();

        auto list = channel()->directoryFileList(deviceDirPath);
        if (Channel::NoError != channel()->errorType() && Channel::FileNotFound != channel()->errorCode()) {
            addError(QString("Не удалось получить список содержимого папки %1").arg(deviceDirPath));
            return false;
        }

        for (auto &entryName : list) {
            const QString localDirPath = QDir(dirPath).filePath(deviceDirName);
            if(!QDir().mkpath(localDirPath)) {
                addError("Не удалось создать папку!");
                return false;
            }

            auto sourceFilePath = QString("%1/%2").arg(deviceDirPath, entryName);
            auto destFilePath = QString("%1/%2").arg(localDirPath, entryName);
            filesToDownload[sourceFilePath] = destFilePath;

        }
    }

    QString confFileName = "conf.zip";
    QString source = QString("0:/%1").arg(confFileName);
    QString dest = QString("%1/%2").arg(m_dirPath, confFileName);
    filesToDownload[source] = dest;

    setCurrentStep(ReadFilesStep);
    m_filesCount = filesToDownload.size();
    m_currentFile = 0;
    for (auto it = filesToDownload.begin(); it != filesToDownload.end(); ++it) {        
        auto source = it->first;
        auto dest = it->second;
        if (!channel()->downloadFile(source, dest)) {
            if(Channel::FileNotFound == channel()->errorCode()) {
                addInfo(QString("Файл %1 не найден!").arg(source));
            }
            else {
                addReport(QString("Не удалось загурзить файл %1 с устройства: %2").arg(source, channel()->errorMsg()));
            }
        }

        ++m_currentFile;
    }

    return true;
}

bool ReportOperation::clearDirectory()
{
    QDir dir(m_dirPath);
    if (!dir.exists()) {
        addError(QString("Не удалось найти папку %1").arg(m_dirPath));
        return false;
    }

    const QStringList entries = dir.entryList(QDir::AllEntries | QDir::NoDotAndDotDot);
    for (const QString &entry : entries) {
        const QString fullPath = dir.filePath(entry);

        if (QFileInfo(fullPath).isDir()) {
            QDir(fullPath).removeRecursively();
        }
        else {
            QFile::remove(fullPath);
        }
    }
    addInfo("Папка очищена!");

    return true;
}
