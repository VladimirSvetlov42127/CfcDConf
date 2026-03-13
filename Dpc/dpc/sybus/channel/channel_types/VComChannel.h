#pragma once

#include <dpc/sybus/channel/Channel.h>

class QSerialPort;

namespace Dpc::Sybus
{
	class DPC_EXPORT VComChannel : public Channel
	{
	public:
		VComChannel(const SettingsType &settings, QObject *parent = nullptr);
		~VComChannel();

		virtual ChannelType type() const override { return VCOM; }
		virtual SettingsType settings() const override;

		virtual bool isConnectedToDevice() const override;
	protected:
		virtual bool onConnect() override;
		virtual bool onDisconnect() override;
		virtual bool onSendData(const QByteArray &data) override;
		virtual QByteArray onReadData() override;

	private:
		void init();
		void clearSerial();
		void abort(uint32_t errorCode = 0, const QString &errorMsg = QString());

	private:
		//quint8 m_comIndex;
		QString m_comName;
		QSerialPort* m_serial;
	};
}