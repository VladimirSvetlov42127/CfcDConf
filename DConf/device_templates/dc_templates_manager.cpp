#include "dc_templates_manager.h"

#include <QFile>
#include <QDir>
#include <QDebug>

#include "utils/cfg_path.h"

namespace {

const char* g_TemplatesDir = "templates";
const char* g_DeviceTemplateFile = "template.db";
const char* g_TemplatesConfFile = "configurations.lst";
const char* g_TemplatesCompatiblesFile = "compatible_devices.txt";

}

DcTemplatesManager &DcTemplatesManager::getInstance()
{
    static DcTemplatesManager tm;
    return tm;
}

QString DcTemplatesManager::path() const
{
    return QString("%1/%2").arg(cfg::path::appRootDir(), g_TemplatesDir);
}

bool DcTemplatesManager::load()
{
    bool result = true;
    result &= loadCompatibleSoftCodes();
    result &= loadDeviceTemplates();
    result &= loadDeprotecModifications();

    return result;
}

bool DcTemplatesManager::isCompatibleDevices(uint16_t softCode1, uint16_t softCode2) const
{
    if (softCode1 == softCode2)
        return true;

    for (auto &codeList : m_compatibleSoftCodes) {
        auto it = codeList.find(softCode1);
        if (it != codeList.end())
            return codeList.find(softCode2) != codeList.end();
    }

    return false;
}

const QMap<QString, QString> &DcTemplatesManager::deviceTemplates() const
{
    return m_deviceTemplates;
}

const QMap<QString, QString> &DcTemplatesManager::deprotecModifications() const
{
    return m_deprotecModifications;
}

QString DcTemplatesManager::deviceTemplateFilePath(const QString &deviceType, const QString &modification) const
{
    if (deviceType.isEmpty())
        return QString();

    QString prefix = QString("%1/%2").arg(path(), deviceType);
    if (!modification.isEmpty())
        prefix = QString("%1/%2").arg(prefix, modification);

    return QString("%1/%2").arg(prefix, g_DeviceTemplateFile);
}

DcTemplatesManager::DcTemplatesManager()
{
}

bool DcTemplatesManager::loadCompatibleSoftCodes()
{
    m_compatibleSoftCodes.clear();

    auto filePath = QString("%1/%2").arg(path(), g_TemplatesCompatiblesFile);
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        emit error(QString("Не удалось открыть файл совместимых устройств %1: %2").arg(filePath, file.errorString()));
        return false;
    }

    for (auto &line : QString(file.readAll()).split('\n')) {
        auto softCodesList = line.split("//");
        if (softCodesList.isEmpty())
            continue;

        SoftCodeSet set;
        for (auto &code : softCodesList.first().split(',')) {
            bool ok = false;
            int val = code.trimmed().toInt(&ok);
            if (!ok)
                continue;

            set.insert(val);
        }

        if (!set.empty())
            m_compatibleSoftCodes.push_back(set);
    }

    return true;
}

bool DcTemplatesManager::loadDeviceTemplates()
{
    m_deviceTemplates.clear();

    //	Проверка списка контроллеров
    QDir model_directory(path());
    if (!model_directory.exists()) {
        emit error(QString("Не найдена база шаблонов"));
        return false;
    }

    QFileInfoList controllers_list = model_directory.entryInfoList(QDir::AllDirs | QDir::NoDotAndDotDot);
    if (controllers_list.isEmpty()) {
        emit error(QString("База шаблонов пуста"));
        return false;
    }

    //	Заполнение списка контроллеров
    for (int i = 0; i < controllers_list.size(); ++i) {
        QFileInfo fileInfo = controllers_list.at(i);
        if (!fileInfo.isDir())
            continue;

        auto type = fileInfo.fileName();
        auto templateFilePath = QString("%1/%2/%3").arg(path(), type, g_DeviceTemplateFile);
        if (type.contains("DEPROTEC-LT")) {
            type = "DEPROTEC";
            templateFilePath.clear();
        }

        m_deviceTemplates[type] = templateFilePath;
    }

    return true;
}

bool DcTemplatesManager::loadDeprotecModifications()
{
    m_deprotecModifications.clear();

    auto filePath = QString("%1/%2").arg(path(), g_TemplatesConfFile);
    QFile list_file(filePath);
    if (!list_file.open(QIODevice::ReadOnly)) {
        emit error(QString("Не удалось откырть файл модификаций DEPROTEC %1: %2").arg(filePath, list_file.errorString()));
        return false;
    }

    QTextStream input(&list_file);
    QString config_string = input.readAll();
    QStringList config_list = config_string.split("\n");
    for (int i = 0; i < config_list.count(); i++) {
        if (config_list.at(i).isEmpty())
            continue;

        QStringList map_list = config_list.at(i).split(";");
        if (map_list.size() < 2)
            continue;

        auto modification = map_list.at(0).trimmed();
        auto modificationDir = map_list.at(1).trimmed();
        auto modificationTemplateFilePath = QString("%1/%2/%3").arg(path(), modificationDir, g_DeviceTemplateFile);
        m_deprotecModifications[modification] = modificationTemplateFilePath;
    }

    return true;
}
