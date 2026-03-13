#pragma once

#include <dpc/sybus/channel/async/AsyncChannel.h>

class QSerialPort;
class QTimer;

namespace Dpc::Sybus
{
	class DPC_EXPORT AsyncVComChannel : public AsyncChannel
	{
		Q_OBJECT

	public:
		AsyncVComChannel(const SettingsType& settings, QObject* parent = nullptr);
		~AsyncVComChannel();

		virtual ChannelType type() const override { return VCOM; }
		virtual SettingsType settings() const override;

		virtual bool isConnectedToDevice() const override;

	protected:
		virtual void doConnectToDevice() override;
		virtual void doDisconnectFromDevice() override;
		virtual void doSendData(const QByteArray& data) override;

	private slots:
		void onSerialAboutToClose();
		void onSerialError();
		void onSerialReadyRead();
		void onTimeout();		

	private:
		void abort(ChannelReply::Error err = ChannelReply::NoError, const QString &errMsg = QString());
        void serialDaraRazbor(const QByteArray& data);
	private:
		//quint8 m_comIndex;
		QString m_comName;

		QSerialPort* m_serial;
		QTimer* m_timer;
		QByteArray m_read_data;
	};
}
