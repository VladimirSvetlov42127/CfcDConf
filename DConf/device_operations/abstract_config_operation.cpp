#include "abstract_config_operation.h"

#include "device_templates/dc_templates_manager.h"

AbstractConfigOperation::AbstractConfigOperation(const QString &name, DcController *device, int stepsCount, QObject *parent)
    : AbstractOperation{name, stepsCount, parent}
    , m_config{device}
    , m_isCheckSoftIdEnabled(false)
{
}

QString AbstractConfigOperation::configArchiveDevicePath()
{
    return QString("0:/conf.zip");
}

DcController *AbstractConfigOperation::config() const
{
    return m_config;
}

void AbstractConfigOperation::setCheckSoftIdEnabled(bool enabled)
{
    m_isCheckSoftIdEnabled = enabled;
}

bool AbstractConfigOperation::checkSoftId()
{
    if (!m_isCheckSoftIdEnabled)
        return true;

    addInfo("Проверка совместимости шаблона конфигурации и устройства...");

    auto p = channel()->param(SP_SOFTWARE_ID);
    if (!p) {
        addError(QString("Не удалось прочитать код ПО с устойства: %1").arg(channel()->errorMsg()));
        return false;
    }
    uint16_t deviceSoftId = p->value<uint16_t>();

    auto param = config()->paramsRegistry().element(SP_SOFTWARE_ID, 0);
    if (!param) {
        addError(QString("Не удалось прочитать код ПО из шаблона конфигурации: %1").arg(channel()->errorMsg()));
        return false;
    }
    uint16_t localSoftId = param->value().toUInt();

    addInfo(QString("Коды ПО: на устройстве %1, в шаблоне конфигурации %2").arg(deviceSoftId).arg(localSoftId));
    if (!gTemplatesManager.isCompatibleDevices(localSoftId, deviceSoftId)) {
        addError("Не совместимые коды ПО");
        return false;
    }

    return true;
}
