#include "TcpChannel.h"

#include <qtcpsocket.h>
#include <qdebug.h>

namespace {
	const uint16_t RESPONSE_TIMEOUT = 10000;
}

namespace Dpc::Sybus
{
	TcpChannel::TcpChannel(const SettingsType & settings, QObject *parent) :
		Channel(settings, parent),
		m_ip(settings.value(IpAddrSetting).toString()),
		m_port(settings.value(IpPortSetting).toUInt()),
		m_currentSyncProtocol(-1)

	{
		init();
	}

	TcpChannel::~TcpChannel()
	{
		onDisconnect();
	}

	SettingsType TcpChannel::settings() const
	{
		auto res = Channel::settings();
		res[IpPortSetting] = m_port;
		res[IpAddrSetting] = m_ip;
		return res;
	}

	bool TcpChannel::onConnect()
	{
        if (isConnectedToDevice())
			return true;

		m_socket = new QTcpSocket;
		m_socket->connectToHost(m_ip, m_port);

		if (!m_socket->waitForConnected(RESPONSE_TIMEOUT)) {
			abort();
			return false;
		}

		return true;
	}

	bool TcpChannel::onDisconnect()
	{
		clearSocket();
		return true;
	}

	bool TcpChannel::onSendData(const QByteArray &data)
	{
        if (!isConnectedToDevice() && !onConnect())
			return false;

		if (m_socket->write(data) < 0) {
			abort();
			return false; }

		if (!m_socket->waitForBytesWritten(RESPONSE_TIMEOUT)) {
			abort(0, QString("SEND: %1").arg(m_socket->errorString()));
			return false;
		}

		return true;
	}

	QByteArray TcpChannel::onReadData()
	{
		if (!m_socket->waitForReadyRead(RESPONSE_TIMEOUT) && !m_socket->bytesAvailable()) {
			abort(0, QString("READ: %1").arg(m_socket->errorString()));
			return QByteArray();
		}		

		auto data = m_socket->read(readBufferSize() * 2);
		if (data.isEmpty()) {
			abort();
		}
		
		return data;
	}

	bool TcpChannel::beginChangeDateTime()
	{
		m_currentSyncProtocol = -1;
		auto syncProtocol = getSyncProtocol();
		if (syncProtocol < 0)
			return false;

		switch (syncProtocol) {
		case IEEE1588_UDP:
		case IEEE1588_ETH:
		case SNTP:
		case IEC_104_TSYNC:
		case SYBUSTCP_TSYNC:
		case MODBUSTCP_TSYNC:
			return true;
		default:break;
		}

		if (!setSyncProtocol(SYBUSTCP_TSYNC))
			return false;

		m_currentSyncProtocol = syncProtocol;
		return true;
	}

	bool TcpChannel::endChangeDateTime()
	{
		if (m_currentSyncProtocol < 0)
			return true;

		return setSyncProtocol(m_currentSyncProtocol);
	}

	void TcpChannel::init()
	{
		m_socket = nullptr;
	}

	bool TcpChannel::isConnectedToDevice() const
	{
		return m_socket;
	}

	void TcpChannel::clearSocket()
	{
		if (!m_socket)
			return;

		m_socket->close();
		m_socket->deleteLater();
		m_socket = nullptr;
	}

	void TcpChannel::abort(uint32_t errorCode, const QString & errorMsg)
	{
		if (!errorCode)
			errorCode = m_socket->error();
		QString msg = errorMsg;
		if (msg.isEmpty())
			msg = m_socket->errorString();

		setError(ChannelErrorType, errorCode, msg);
		clearSocket();
	}	
} // namespace
