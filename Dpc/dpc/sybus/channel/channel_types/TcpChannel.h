#pragma once

#include <dpc/sybus/channel/Channel.h>

class QTcpSocket;

namespace Dpc::Sybus
{
	class DPC_EXPORT TcpChannel : public Channel
	{
	public:
		TcpChannel(const SettingsType &settings, QObject *parent = nullptr);
		~TcpChannel();

		virtual ChannelType type() const override { return TCP; }
		virtual SettingsType settings() const override;

        virtual bool isConnectedToDevice() const override;
	protected:
		virtual bool onConnect() override;
		virtual bool onDisconnect() override;
		virtual bool onSendData(const QByteArray &data) override;
		virtual QByteArray onReadData() override;

		virtual bool beginChangeDateTime() override;
		virtual bool endChangeDateTime() override;

	private:
		void init();
		void clearSocket();
		void abort(uint32_t errorCode = 0, const QString &errorMsg = QString());

	private:
		QTcpSocket* m_socket;

		QString m_ip;
		uint16_t m_port;
		int8_t m_currentSyncProtocol;
	};
} // namespace
