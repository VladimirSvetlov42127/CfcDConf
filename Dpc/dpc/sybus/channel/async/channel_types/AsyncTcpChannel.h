#pragma once

#include <dpc/sybus/channel/async/AsyncChannel.h>

class QTcpSocket;
class QTimer;

namespace Dpc::Sybus
{
	class DPC_EXPORT AsyncTcpChannel : public AsyncChannel
	{
		Q_OBJECT

	public:
		AsyncTcpChannel(const SettingsType& settings, QObject* parent = nullptr);
		~AsyncTcpChannel();

		virtual ChannelType type() const override { return TCP; }
		virtual SettingsType settings() const override;

		virtual bool isConnectedToDevice() const override;		

	protected:
		virtual void doConnectToDevice() override;
		virtual void doDisconnectFromDevice() override;
		virtual void doSendData(const QByteArray& data) override;

	private slots:
		void onSocketConnected();
		void onSocketDisconnected();
		void onSocketError();
		void onSocketReadyRead();
		void onTimeout();

	private:
		void abort(const QString &errMsg = QString());

	private:
		QString m_ip;
		uint16_t m_port;

		QTcpSocket* m_socket;
		QTimer* m_timer;
	};
} // namespace