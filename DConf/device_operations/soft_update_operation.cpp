#include "soft_update_operation.h"

#include <QFileDialog>

#include <dpc/sybus/channel/Channel.h>

using namespace Dpc::Sybus;

namespace {

enum OperationSteps {
    UpdateStep = 0,

    TotalStepsCount
};

} // namespace

SoftUpdateOperation::SoftUpdateOperation(const QString &fileName, QObject * parent)
    : AbstractOperation("Обновление устройства", TotalStepsCount, parent)
    , m_fileName(fileName)
{
}

void SoftUpdateOperation::onChannelError(Dpc::Sybus::Channel::ErrorType errorType, int errorCode, const QString &errorMsg)
{
    addError(channel()->errorMsg());
}

void SoftUpdateOperation::onChannelProgress(int state)
{
    this->emitProgress(state);
}

bool SoftUpdateOperation::exec()
{
	setCurrentStep(UpdateStep);
    if (!channel() || !channel()->updateDevice(m_fileName)) {
		addError(QString("Не удалось обновить устройство."));
		return false;
	}

    return true;
}
