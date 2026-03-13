#include "information_operation.h"
#include "data_model/dc_controller.h"

#include <dpc/sybus/channel/Channel.h>
#include <dpc/sybus/utils.h>

using namespace Dpc::Sybus;

namespace {

const int CRITICAL_ERROR = 0X0A30;
const int DINAMIC_DATA = 0X0830;

enum OperationSteps {
    ReadParamStep = 0,

    TotalStepsCount
};
}

InformationOperation::InformationOperation(const QList<uint16_t> &requiredParams,  QObject *parent)
    : AbstractOperation("Получение параметров", TotalStepsCount, parent)
    , m_requiredParams(requiredParams)
{
}

const InformationOperation::ParamsContainer &InformationOperation::params() const
{
    return m_params;
}

void InformationOperation::onChannelError(Dpc::Sybus::Channel::ErrorType errorType, int errorCode, const QString &errorMsg)
{
    addError(channel()->errorMsg());
}

bool InformationOperation::exec()
{
    if (!readParams())
        return false;

    return true;
}

bool InformationOperation::readParams()
{
    setCurrentStep(ReadParamStep, m_requiredParams.size());
    int progress = 0;

    for (int i = 0; i < m_requiredParams.size(); ++i) {
        emitProgress(++progress);
        auto addr = m_requiredParams.value(i);

        auto dim = channel()->dimension(addr);
        if (!dim) {
            if (Channel::DeviceErrorType == channel()->errorType() && addr == SP_IP4_ADDR)
                continue;
            if (addr != SP_IP4_ADDR)
                continue;
            // if (neededParams[it.key()] == false) continue;
            addError(QString("Не удалось получить размерность параметра %1: %2").arg(toHex(addr)).arg(channel()->errorMsg()));
            return false;
        }

        auto dimProfileSize = dim.profileCount();
        auto dimSize = dim.subProfileCount();
        QStringList values;

        if (dimProfileSize > 1) {
            // values.append(channel()->param(addr));
            for(size_t i = 0; i < dimProfileSize; ++i) {
                uint16_t j = 256;
                for(size_t g = 0; g < dimSize; ++g) {
                    values.append(channel()->param(addr, g + (i * j)).get()->value<QString>());
                }
            }
        }
        else {
            for(size_t j = 0; j < dimSize; ++j) {
                values.append(channel()->param(addr, j).get()->toString());
            }
        }

        if (Channel::NoError != channel()->errorType()) {
            addError(QString("Не удалось получить значения параметра %1: %2").arg(toHex(addr)).arg(channel()->errorMsg()));
            return false;
        }

        auto namesPack = channel()->names(addr);
        if (Channel::NoError != channel()->errorType()) {
            addError(QString("Не удалось получить имена параметра %1: %2").arg(toHex(addr)).arg(channel()->errorMsg()));
            return false;
        }

        ParamElements element;
        ParamProfiles profiles;
        for (size_t i = 0; i < dimSize * dimProfileSize; ++i) {
            QString name = namesPack->value<QString>();

            if (namesPack->count() == dimSize + 1 || dimProfileSize > 1)
                name = namesPack->value<QString>(i + 1);

            Element item = {name, values[i]};

            element.emplace(i, item);

            if (element.size() == dimSize) {
                profiles.push_back(element);
                element.clear();
            }
        }

        m_params[addr] = profiles;
    }

    if (channel()->dateTime().isValid()){
        m_dateTime = channel()->dateTime();
        if (!m_dateTime.isValid())
            return false;

        m_offset = channel()->dateTimeOffset();
        if (m_offset.isNull() && Dpc::Sybus::Channel::NoError != channel()->errorCode()) {
            //		return false;
        }
    }

    return true;
}
