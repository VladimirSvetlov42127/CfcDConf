#include "dc_operation_handler.h"

#include <QApplication>

#define TRACE(msg) \
    qDebug().noquote() << QString("[OH] %1").arg(msg);

DcOperationHandler::DcOperationHandler(QObject *parent)
    : QObject(parent)
    , m_workerThread{nullptr}
    , m_currentOperation{nullptr}
    , m_isBusy{false}
{
}

DcOperationHandler::~DcOperationHandler()
{
    cleanUp();
//    qDebug() << "~DcOperationHandler";
}

void DcOperationHandler::setChannel(Dpc::Sybus::ChannelPtr channel)
{
    cleanUp();
    m_channel = channel;
    if (!channel)
        return;

    m_workerThread = new QThread(this);
    channel->moveToThread(m_workerThread);
    connect(m_workerThread, &QThread::finished, m_channel.get(), &Dpc::Sybus::Channel::deleteLater);
    m_workerThread->start();
}

const Dpc::Sybus::Channel *DcOperationHandler::channel() const
{
    return m_channel.get();
}

bool DcOperationHandler::isBusy() const
{
    return currentOperation();
}

bool DcOperationHandler::exec(AbstractOperation::SPtr operation)
{    
    if (!m_workerThread && isBusy())
        return false;

//    TRACE(QString("started: %1").arg(operation->name()));
    operation->moveToThread(m_workerThread);
    operation->setChannel(m_channel);
    connect(operation.get(), &AbstractOperation::infoMsg, this, &DcOperationHandler::infoMsg);
    connect(operation.get(), &AbstractOperation::debugMsg, this, &DcOperationHandler::debugMsg);
    connect(operation.get(), &AbstractOperation::warningMsg, this, &DcOperationHandler::warningMsg);
    connect(operation.get(), &AbstractOperation::errorMsg, this, &DcOperationHandler::errorMsg);
    connect(operation.get(), &AbstractOperation::progress, this, &DcOperationHandler::progress);
    connect(operation.get(), &AbstractOperation::finished, this, &DcOperationHandler::onOperationFinished);
    connect(m_workerThread, &QThread::finished, operation.get(), &AbstractOperation::deleteLater);

    m_currentOperation = operation;
    QMetaObject::invokeMethod(operation.get(), &AbstractOperation::start, Qt::QueuedConnection);
    return true;
}

void DcOperationHandler::onOperationFinished([[maybe_unused]] int state)
{
    auto operation = currentOperation();
    QMetaObject::invokeMethod(operation, [this, operation]() {
        operation->moveToThread(this->thread());
        QMetaObject::invokeMethod(this, [this](){ this->finishCurrentOperation(); }, Qt::QueuedConnection);
    }, Qt::QueuedConnection);
}

void DcOperationHandler::cleanUp()
{
    if (!m_workerThread)
        return;

    if (m_channel) {
        m_channel->disconnect();
    }

    m_workerThread->quit();
    m_workerThread->wait();
    m_workerThread->deleteLater();
    m_workerThread = nullptr;
}

AbstractOperation *DcOperationHandler::currentOperation() const
{
    return m_currentOperation.get();
}

void DcOperationHandler::finishCurrentOperation()
{
    auto operation = m_currentOperation;
    m_currentOperation.reset();

//    TRACE(QString("finished: %1").arg(operation->name()));
    emit finished(operation.get());
    operation->deleteLater();
}
