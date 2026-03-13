#include "AsyncVComChannel.h"
#include "dpc/dep_settings.h"

#include <qdebug.h>
#include <qserialport.h>
#include <qtimer.h>

#include <dpc/sybus/utils.h>

namespace {
	const uint16_t RESPONSE_TIMEOUT = 1200;

	char HexToAscii(uint8_t data)
	{
		if (data < 10)
			return data + '0';
		else
			return data - 10 + 'A';
	}

	char HexToAscii(uint8_t data, bool secondHalf)
	{
		if (secondHalf)
			data = (data >> 4) & 0x0F;
		else
			data = data & 0x0F;

		return HexToAscii(data);
	}

	uint8_t AsciiToHex(char d)
	{
		if ((d >= '0') && (d <= '9'))
			return d - '0';

		if ((d >= 'a') && (d <= 'f'))
			return d - 'a' + 10;

		if ((d >= 'A') && (d <= 'F'))
			return d - 'A' + 10;

		return 0xFF;
	}
} // namespace

namespace Dpc::Sybus
{
	AsyncVComChannel::AsyncVComChannel(const SettingsType & settings, QObject *parent) :
		AsyncChannel(parent),
		//m_comIndex(settings.value(ComPortSetting).toUInt()),
        m_comName(settings.value(com_port_name).toString()),
		m_serial(new QSerialPort(m_comName, this)),
		m_timer(new QTimer(this)), 
		m_read_data(QByteArray())
	{
		m_timer->setInterval(RESPONSE_TIMEOUT);

		connect(m_timer, &QTimer::timeout, this, &AsyncVComChannel::onTimeout);
		connect(m_serial, &QSerialPort::aboutToClose, this, &AsyncVComChannel::onSerialAboutToClose);
		connect(m_serial, &QSerialPort::errorOccurred, this, &AsyncVComChannel::onSerialError);
		connect(m_serial, &QSerialPort::readyRead, this, &AsyncVComChannel::onSerialReadyRead);
	}

	AsyncVComChannel::~AsyncVComChannel()
	{
	}

	SettingsType AsyncVComChannel::settings() const
	{
		auto res = AsyncChannel::settings();
		//res[ComPortSetting] = m_comIndex;
        res[com_port_name] = m_comName;		return res;
	}

	bool AsyncVComChannel::isConnectedToDevice() const
	{
		return m_serial->isOpen();
	}

	void AsyncVComChannel::doConnectToDevice()
	{
		if (isConnectedToDevice())
			return;

		if (!m_serial->open(QIODevice::ReadWrite)) {
			return;
		}
		
		m_serial->setBaudRate(QSerialPort::Baud38400);
		m_serial->setDataBits(QSerialPort::Data8);
		m_serial->setStopBits(QSerialPort::OneStop);
		m_serial->setParity(QSerialPort::NoParity);

		onConnectedToDevice();
	}

	void AsyncVComChannel::doDisconnectFromDevice()
	{
		if (!isConnectedToDevice())
			return;

		m_serial->close();
	}

	void AsyncVComChannel::doSendData(const QByteArray& data)
	{
        m_read_data.clear();

		QByteArray pack;
		for (size_t i = 0; i < data.size(); i++)
		{
			char c = data.at(i);
			pack.append(HexToAscii(c, true));
			pack.append(HexToAscii(c, false));
		}

		auto crc = Sybus::crc16((uint8_t*)data.data(), data.size(), 0);
		pack.append(HexToAscii(crc, true));
		pack.append(HexToAscii(crc, false));
		pack.append(HexToAscii(crc >> 8, true));
		pack.append(HexToAscii(crc >> 8, false));
		pack.append((char)0);

		addInfoMsg(QString("[USB] SEND (%1) -> ").arg(pack.size()).append(pack.toHex(':')).toUpper());
		if (m_serial->write(pack) < 0) {
			abort();
			return;
		}

		m_serial->flush();
		m_timer->start();
	}

	void AsyncVComChannel::onSerialAboutToClose()
	{
		m_timer->stop();
		onDisconnectedFromDevice();
	}

	void AsyncVComChannel::onSerialError()
	{
		if (QSerialPort::NoError == m_serial->error())
			return;

		abort();
	}

	void AsyncVComChannel::onSerialReadyRead()
    {
        // Если сработал сигнал readyRead, но в SerialPort нет данных - скорее всего возникла ошибка.
        // В abort без аргументов установится нужный тип ошибки.
		auto pack = m_serial->readAll();
		if (pack.isEmpty()) {
			abort();
			return;
		}

        // Если нет признака конца пакета('\0'), перезапускаем таймер и ждем новую порцию данных.
		m_read_data += pack;
        if (0 != m_read_data.back()) {
            m_timer->start();
            return;
        }

        addInfoMsg(QString("[USB] RECV (%1) <- ").arg(pack.size()).append(pack.toHex(':')).toUpper());

        // Останавливаем таймер, удаляем последний символ(признак конца пакета) и отправляем пакет на разбор.
        m_timer->stop();
        m_read_data.chop(1);
        serialDaraRazbor(m_read_data);
	}

    void AsyncVComChannel::serialDaraRazbor(const QByteArray& pack)
	{
		// распаковка ASCII to HEX
		QByteArray data;
		for (size_t i = 0; i < pack.size(); i++) {
			auto c = AsciiToHex(pack.at(i));
			if (c == 0xFF) {
				abort(ChannelReply::InvalidASCII);
				return;
			}

			if ((i % 2))
				data[data.size() - 1] = (uint8_t)data.back() | c;
			else
				data.append(c << 4);
		}

		// проверка CRC(последние 2 байта)
		uint16_t recivedCRC = ((uint8_t)data[data.size() - 1] << 8) | (uint8_t)data[data.size() - 2];
		data.chop(2);
		uint16_t calculatedCRC = Sybus::crc16((uint8_t*)data.data(), data.size(), 0);
		if (recivedCRC != calculatedCRC) {
			abort(ChannelReply::InvalidCRC);
			return;
		}

		onReceivedData(data);
	}

	void AsyncVComChannel::onTimeout()
	{
		abort(ChannelReply::TransportError, "Operation timeout");
	}

	void AsyncVComChannel::abort(ChannelReply::Error err, const QString& errMsg)
	{
		m_timer->stop();

		err = ChannelReply::NoError == err ? ChannelReply::TransportError : err;
		QString msg = errMsg;
		if (msg.isEmpty() && ChannelReply::TransportError == err) {
				msg = m_serial->errorString();
		}
		
		setError(err, msg);
	}

} // namespace
