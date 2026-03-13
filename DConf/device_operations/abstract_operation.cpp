#include "abstract_operation.h"

#include <qapplication.h>
#include <qdebug.h>

#include <dpc/sybus/channel/Channel.h>

using namespace Dpc::Sybus;

AbstractOperation::AbstractOperation(const QString &name, int stepsCount, QObject *parent) :
	QObject(parent),
    m_name(name),
	m_state(NoErrorState),
    m_stepsCount(stepsCount)
{
//    qRegisterMetaType<AbstractOperation::SPtr>();
    setCurrentStep(0);
}

AbstractOperation::~AbstractOperation()
{
//    qDebug() << "~Operation" << name();
}

QString AbstractOperation::name() const
{
    return m_name;
}

AbstractOperation::State AbstractOperation::state() const
{
	return m_state;
}

const QStringList &AbstractOperation::reportList() const
{
    return m_reportsList;
}

bool AbstractOperation::hasProgress() const
{
    return m_stepsCount;
}

ChannelPtr AbstractOperation::channel() const
{
    return m_channel;
}

void AbstractOperation::setChannel(Dpc::Sybus::ChannelPtr channel)
{
    if (m_channel == channel)
        return;

    if (m_channel) {
        disconnect(m_channel.get(), &Channel::debug, this, &AbstractOperation::onChannelDebug);
        disconnect(m_channel.get(), &Channel::info, this, &AbstractOperation::onChannelInfo);
        disconnect(m_channel.get(), &Channel::error, this, &AbstractOperation::onChannelError);
        disconnect(m_channel.get(), &Channel::progress, this, &AbstractOperation::onChannelProgress);
    }

    m_channel = channel;
    connect(m_channel.get(), &Channel::debug, this, &AbstractOperation::onChannelDebug);
    connect(m_channel.get(), &Channel::info, this, &AbstractOperation::onChannelInfo);
    connect(m_channel.get(), &Channel::error, this, &AbstractOperation::onChannelError);
    connect(m_channel.get(), &Channel::progress, this, &AbstractOperation::onChannelProgress);
}

void AbstractOperation::start()
{
	addInfo(QString("Старт операции '%1'").arg(m_name));
	m_timer.start();

	if (!channel()) {
		abort("Не задано соединение");
		return;
	}	

	if (!before()) {
		abort();
		return;
	}

	if (!channel()->connect()) {
        abort(QString("Не удалось установить соединение c устройством: %1").arg(channel()->errorMsg()));
		return;
	}

	if (!exec()) {
		abort();
		return;
	}

	if (!channel()->disconnect()) {
        abort(QString("Не удалось корректно разорвать соединение c устройством: %1").arg(channel()->errorMsg()));
		return;
	}

	if (!after()) {
		abort();
		return;
	}

	setCurrentStep(m_stepsCount);
	finish(reportList().isEmpty() ? NoErrorState : WarningState);
}

void AbstractOperation::addReport(const QString & report, bool warning)
{
	m_reportsList.append(report);
	if (warning)
		addWarning(report);
}

void AbstractOperation::addInfo(const QString & msg)
{
    emit infoMsg(msg);
}

void AbstractOperation::addWarning(const QString & msg)
{
    emit warningMsg(msg);
}

void AbstractOperation::addError(const QString & msg)
{
    emit errorMsg(msg);
}

void AbstractOperation::addDebug(const QString & msg, int level)
{
    emit debugMsg(msg, level);
}

void AbstractOperation::emitProgress(int currentStepValue)
{
    if (!hasProgress())
        return;

    auto totalProgressPerStep = 100.0 / m_stepsCount;
    int currentStepProgress = static_cast<double>(currentStepValue) / m_currentStepTotal * totalProgressPerStep;
    int currentStepOffset = static_cast<double>(m_currentStep) / m_stepsCount * 100;
    auto value = currentStepOffset + currentStepProgress;
    emit progress(value);
}

bool AbstractOperation::saveConfig(int board)
{
    if (!channel()->saveConfig(board)) {
		addError(QString("Не удалось сохранить конфигурацию: %1").arg(channel()->errorMsg()));
		return false;
	}

	return true;
}

bool AbstractOperation::restart(int board)
{
    if (!channel()->restartDevice(Channel::HardReset, board)) {
		addError(QString("Не удалось выполнить перезагрузку: %1").arg(channel()->errorMsg()));
		return false;
	}

	return true;
}

void AbstractOperation::onChannelDebug(int level, const QString &msg)
{
    addDebug(msg, level);
}

void AbstractOperation::onChannelInfo(const QString& msg)
{
    addInfo(msg);
}

void AbstractOperation::onChannelError(Dpc::Sybus::Channel::ErrorType errorType, int errorCode, const QString &errorMsg)
{
}

void AbstractOperation::onChannelProgress(int state)
{
}

void AbstractOperation::abort(const QString & msg)
{
	if (!msg.isEmpty())
		addError(msg);

	finish(ErrorState);
}

void AbstractOperation::setCurrentStep(int step, int total)
{
    if (!hasProgress())
        return;

    m_currentStep = step;
    m_currentStepTotal = total;
    emitProgress(0);
}

void AbstractOperation::finish(AbstractOperation::State state)
{
	if (channel()->thread() != qApp->thread())
		channel()->moveToThread(nullptr);

	m_state = state;
	QString msg = QString("Операция '%1' завершилась %3, время: %2 секунды").arg(m_name).arg(m_timer.elapsed() / 1000.0);
	switch (state)
	{
    case AbstractOperation::NoErrorState: addInfo(msg.arg("успешно"));	break;
    case AbstractOperation::WarningState: addWarning(msg.arg("с предупреждениями")); break;
    case AbstractOperation::ErrorState: addError(msg.arg("неудачно"));	break;
	default: return;
	}

	emit finished(state);
}
