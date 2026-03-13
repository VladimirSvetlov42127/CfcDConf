#pragma once

#include <qobject.h>

class QProcess;

class DcUpdater : public QObject
{
	Q_OBJECT

public:
	explicit DcUpdater(QObject* parent = nullptr);
	~DcUpdater();	

signals:
	void error(const QString& errorMsg);
	void updatesAvailableState(bool state);

public slots:
	void checkUpdates();
	bool startUpdates();

private slots:
	void onProcessError();
	void onProcessFinished(int exitCode);
	void onReadyReadStandard();
	void onReadyReadError();	

private:
	QProcess* m_process;
	QByteArray m_data;
};
