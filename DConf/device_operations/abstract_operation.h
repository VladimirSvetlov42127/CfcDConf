#pragma once

#include <memory>

#include <QObject>
#include <QElapsedTimer>

#include <dpc/sybus/channel/Channel.h>

class AbstractOperation : public QObject
{
	Q_OBJECT

public:
    // Тип состояиня опреации
	enum State
	{
		NoErrorState,
		WarningState,
		ErrorState
	};

    // Тип флагов характеристик операции
    enum Flag {
        NoFlag = 0x0,
        Read = 0x1,
        Write = 0x2,
        Filesystem = 0x4
    };
    Q_DECLARE_FLAGS(Flags, Flag);

    using SPtr = std::shared_ptr<AbstractOperation>;

    AbstractOperation(const QString &name, int stepsCount = 0,  QObject *parent = nullptr);
    virtual ~AbstractOperation();

    QString name() const;
	State state() const;
    const QStringList& reportList() const;

    // Если ли у операции прогрес
    bool hasProgress() const;

    Dpc::Sybus::ChannelPtr channel() const;
    void setChannel(Dpc::Sybus::ChannelPtr channel);

    virtual Flags flags() const = 0;

signals:
    // сигналы от объекта операции
	void progress(int value);
	void finished(int state);

    void infoMsg(const QString &msg);
    void warningMsg(const QString &msg);
    void errorMsg(const QString &msg);
    void debugMsg(const QString &msg, int level);

public slots:
	void start();

protected slots:
    // Генерация сигналов из классов наследников.
	void addReport(const QString &report, bool warning = true);
	void addInfo(const QString &msg);
	void addWarning(const QString &msg);
	void addError(const QString &msg);
	void addDebug(const QString &msg, int level = 0);
	void emitProgress(int currentStepValue);

    bool saveConfig(int board = -1);
    bool restart(int board = -1);

    // Слоты для сигалов от channel
    virtual void onChannelDebug(int level, const QString &msg);
    virtual void onChannelInfo(const QString& msg);
    virtual void onChannelError(Dpc::Sybus::Channel::ErrorType errorType, int errorCode, const QString &errorMsg);
    virtual void onChannelProgress(int state);

protected:
	void abort(const QString &msg = QString());
    void setCurrentStep(int step, int total = 100);

	virtual bool before() { return true; }
	virtual bool exec() = 0;
	virtual bool after() { return true; }	

private:
    void finish(AbstractOperation::State state);	

private:
    QString m_name;
    Dpc::Sybus::ChannelPtr m_channel;
	QStringList m_reportsList;
    AbstractOperation::State m_state;

	int m_stepsCount;
	int m_currentStep;
	int m_currentStepTotal;	

	QElapsedTimer m_timer;
};
Q_DECLARE_METATYPE(AbstractOperation::SPtr)
Q_DECLARE_OPERATORS_FOR_FLAGS(AbstractOperation::Flags)
