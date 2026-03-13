#ifndef DCOPERATIONHANDLER_H
#define DCOPERATIONHANDLER_H

#include <QObject>
#include <QThread>

#include <dpc/sybus/channel/Channel.h>

#include "device_operations/abstract_operation.h"

class DcOperationHandler : public QObject
{
    Q_OBJECT
public:
    DcOperationHandler(QObject *parent = nullptr);
    ~DcOperationHandler();

    // Установка канала связи, по которому будут выполняться операции
    void setChannel(Dpc::Sybus::ChannelPtr channel);
    const Dpc::Sybus::Channel* channel() const;

    // Занято, выполняется операция
    bool isBusy() const;

    // Начать выполнение операции
    bool exec(AbstractOperation::SPtr operation);

signals:
    // Выполнение operation завершенно. operation удалится по завершению метода
    void finished(AbstractOperation* operation);

    // Прогресс (ход) выполнения операции
    void progress(int value);

    void infoMsg(const QString &msg);
    void warningMsg(const QString &msg);
    void errorMsg(const QString &msg);
    void debugMsg(const QString &msg, int level);

private slots:
    void onOperationFinished(int state);

private:
    // Чистка внутрененго состояние
    void cleanUp();

    AbstractOperation* currentOperation() const;
    void finishCurrentOperation();

private:
    Dpc::Sybus::ChannelPtr m_channel;
    QThread* m_workerThread;
    AbstractOperation::SPtr m_currentOperation;
    bool m_isBusy;
};

#endif // DCOPERATIONHANDLER_H
