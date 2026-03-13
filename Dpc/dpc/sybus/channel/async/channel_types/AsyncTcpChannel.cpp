#include "AsyncTcpChannel.h"

#include <qtcpsocket.h>
#include <qtimer.h>
#include <qdebug.h>

namespace {
	const uint16_t RESPONSE_TIMEOUT = 10000;
}

namespace Dpc::Sybus
{
	AsyncTcpChannel::AsyncTcpChannel(const SettingsType & settings, QObject *parent) :
		AsyncChannel(parent),
		m_ip(settings.value(IpAddrSetting).toString()),
		m_port(settings.value(IpPortSetting).toUInt()),
		m_socket(new QTcpSocket(this)),
		m_timer(new QTimer(this))		
	{
		m_timer->setInterval(RESPONSE_TIMEOUT);

		connect(m_timer, &QTimer::timeout, this, &AsyncTcpChannel::onTimeout);
		connect(m_socket, &QTcpSocket::connected, this, &AsyncTcpChannel::onSocketConnected);
		connect(m_socket, &QTcpSocket::disconnected, this, &AsyncTcpChannel::onSocketDisconnected);
		connect(m_socket, &QTcpSocket::errorOccurred, this, &AsyncTcpChannel::onSocketError);
		connect(m_socket, &QTcpSocket::readyRead, this, &AsyncTcpChannel::onSocketReadyRead);
	}

	AsyncTcpChannel::~AsyncTcpChannel()
	{
	}

	SettingsType AsyncTcpChannel::settings() const
	{
		auto res = AsyncChannel::settings();
		res[IpPortSetting] = m_port;
		res[IpAddrSetting] = m_ip;
		return res;
	}

	bool AsyncTcpChannel::isConnectedToDevice() const
	{
		return m_socket->state() == QAbstractSocket::ConnectedState;
	}

	void AsyncTcpChannel::doConnectToDevice()
	{
		if (QAbstractSocket::HostLookupState == m_socket->state() ||
			QAbstractSocket::ConnectingState == m_socket->state() || 
			QAbstractSocket::ConnectedState == m_socket->state())
			return;

		m_socket->connectToHost(m_ip, m_port);
	}

	void AsyncTcpChannel::doDisconnectFromDevice()
	{
		if (QAbstractSocket::ClosingState == m_socket->state() ||
			QAbstractSocket::UnconnectedState == m_socket->state())
			return;

		m_socket->disconnectFromHost();
	}

	void AsyncTcpChannel::doSendData(const QByteArray& data)
	{
		if (m_socket->write(data) < 0) {
			abort();
			return;
		}

		m_socket->flush();
		m_timer->start();
	}

	void AsyncTcpChannel::onSocketConnected()
	{
		onConnectedToDevice();
	}

	void AsyncTcpChannel::onSocketDisconnected()
	{
		m_timer->stop();
		onDisconnectedFromDevice();
	}

	void AsyncTcpChannel::onSocketError()
	{
		abort();
	}

	void AsyncTcpChannel::onSocketReadyRead()
	{
		m_timer->stop();

		// Чтение данных из сокета. !!!Как проверить, что получены все данные???!!!
		auto data = m_socket->readAll();
		if (data.isEmpty()) {
			abort();
			return;
		}

		onReceivedData(data);
	}

	void AsyncTcpChannel::onTimeout()
	{
		abort("Operation timeout");
	}

	void AsyncTcpChannel::abort(const QString& errMsg)
	{
		m_timer->stop();
		setError(ChannelReply::TransportError, errMsg.isEmpty() ? m_socket->errorString() : errMsg);
	}

} // namespace