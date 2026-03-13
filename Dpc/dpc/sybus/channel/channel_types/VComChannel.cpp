#include "VComChannel.h"
#include "dpc/dep_settings.h"

#include <qserialport.h>

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
	VComChannel::VComChannel(const SettingsType & settings, QObject *parent) :
		Channel(settings, parent),
		//m_comIndex(settings.value(com_port).toUInt()),
        m_comName(settings.value(com_port_name).toString())
	{
		init();
	}

	VComChannel::~VComChannel()
	{
		onDisconnect();
	}

	SettingsType VComChannel::settings() const
	{
		auto res = Channel::settings();
		//res[com_port] = m_comIndex;
        res[com_port_name] = m_comName;
		return res;
	}

	bool VComChannel::onConnect()
	{
        if (isConnectedToDevice())
			return true;

		m_serial = new QSerialPort(m_comName);
		if (!m_serial->open(QIODevice::ReadWrite)) {
			abort();
			return false;
		}

		m_serial->setBaudRate(QSerialPort::Baud38400);
		m_serial->setDataBits(QSerialPort::Data8);
		m_serial->setStopBits(QSerialPort::OneStop);
		m_serial->setParity(QSerialPort::NoParity);

		return true;
	}

	bool VComChannel::onDisconnect()
	{
		clearSerial();
		return true;
	}

	bool VComChannel::onSendData(const QByteArray & data)
	{
        if (!isConnectedToDevice() && !onConnect())
			return false;

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

		DEBUG_LEVEL_UP

		addDebugMsg(QString("[USB]: SEND (%1) -> ").arg(pack.size()).append(pack.toHex(':')).toUpper());
		if (m_serial->write(pack) < 0) {
			abort();
			return false;
		}
		
		if (!m_serial->waitForBytesWritten(RESPONSE_TIMEOUT)) {
			abort(0, QString("SEND: %1").arg(m_serial->errorString()));
			return false;
		}

		return true;
	}

	QByteArray VComChannel::onReadData()
	{
		// размер буфера = (буффер чтения + 2 байта СRC) * 2 + 1 байт 0x0 (конец пакета)
		QByteArray pack;
		pack.clear();
		auto maxDataSize = (readBufferSize() + 2) * 2 + 1;

			while (true)
			{
				if (!m_serial->waitForReadyRead(RESPONSE_TIMEOUT) /*&& !m_serial->bytesAvailable()*/) {
					abort(0, QString("READ: %1").arg(m_serial->errorString()));
					return QByteArray();
				}
				//TODO check readBufferSize (проверка на склеивание, но буфер может быть не правильным)
				pack += m_serial->read(maxDataSize); 
				if (!pack.back())
					break;
			}

		if (pack.isEmpty()) {
			abort();
			return QByteArray();
		}

		DEBUG_LEVEL_UP
		addDebugMsg(QString("[USB]: RECV (%1) <- ").arg(pack.size()).append(pack.toHex(':')).toUpper());

		// Проверка последнего символа на равенство концу пакета(0)		
		if (pack.back()) {
			setError(SystemErrorType, InvalidEOF, "[USB]: некорректный символ конца пакета");
			return QByteArray();
		}

		// удаляем последний смвол, признак конца.
		pack.chop(1);

		// распаковка ASCII to HEX
		QByteArray data;
		for (size_t i = 0; i < pack.size(); i++) {
			auto c = AsciiToHex(pack.at(i));
			if (c == 0xFF) {
				setError(SystemErrorType, InvalidASCII, "[USB]: некорректный ASCII символ");
				return QByteArray();
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
			setError(SystemErrorType, InvalidCRC, "[USB]: некорректный CRC");
			return QByteArray();
		}

		return data;
	}

	void VComChannel::init()
	{
		m_serial = nullptr;
	}

	bool VComChannel::isConnectedToDevice() const
	{
		return m_serial;
	}

	void VComChannel::clearSerial()
	{
        if (!isConnectedToDevice())
			return;

		m_serial->close();
		m_serial->deleteLater();
		m_serial = nullptr;
	}

	void VComChannel::abort(uint32_t errorCode, const QString &errorMsg)
	{
		if (!errorCode)
			errorCode = m_serial->error();
		QString msg = errorMsg;
		if (msg.isEmpty())
			msg = m_serial->errorString();

		setError(ChannelErrorType, errorCode, msg);
		clearSerial();
	}
} // namespace
