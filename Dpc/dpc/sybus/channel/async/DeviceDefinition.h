#pragma once

#include <qbytearray.h>

namespace Dpc::Sybus::Device
{
	const uint8_t MSG_TYPE_REQUEST = 0x0;
	const uint8_t MSG_TYPE_RESPONSE = 0x1;

	const uint16_t REQUEST_WRITE = 0x01;
	const uint16_t REQUEST_WRITE_EXTENDED = 0x41;
	const uint16_t REQUEST_READ = 0x21;
	const uint16_t REQUEST_READ_EXTENDED = 0x61;
	const uint16_t REQUEST_ENCAPSULATE = 0x1E;	

	// Структуры для взаимодействия с устройством. Задаются протоколом взаимодействия с устройством.

#pragma pack(push, 1)
	struct BaseRequest
	{
		uint8_t m_code;
		uint16_t m_addr;
	};

	struct ReadRequest : BaseRequest
	{
		ReadRequest(uint16_t addr, uint16_t index, uint8_t count) :
			BaseRequest{ REQUEST_READ, addr }, m_count(count), m_index(index) {};

		uint8_t m_count;
		uint16_t m_index;

		QByteArray toByteArray() const { return QByteArray((const char*)this, sizeof(ReadRequest)); }
	};

	struct ExtendedReadRequest : BaseRequest
	{
		ExtendedReadRequest(uint16_t addr, uint16_t subAddr, uint16_t index, uint8_t count) :
			BaseRequest{ REQUEST_READ_EXTENDED, addr }, m_subAddr(subAddr), m_count(count), m_index(index) {};

		uint16_t m_subAddr;
		uint8_t m_count;
		uint16_t m_index;

		QByteArray toByteArray() const { return QByteArray((const char*)this, sizeof(ExtendedReadRequest)); }
	};

	struct WriteRequest : BaseRequest
	{
		WriteRequest() = default;
		WriteRequest(uint16_t addr, uint16_t index, uint8_t type, uint8_t count, const QByteArray& data) :
			BaseRequest{ REQUEST_WRITE, addr }, m_type(type), m_count(count), m_index(index), m_data(data) {};

		uint8_t m_type;
		uint8_t m_count;
		uint16_t m_index;
		QByteArray m_data;

		QByteArray toByteArray() const { return QByteArray((const char*)this, sizeof(WriteRequest) - sizeof(QByteArray)) + m_data; }
	};

	struct ExtendedWriteRequest : BaseRequest
	{
		ExtendedWriteRequest() = default;
		ExtendedWriteRequest(uint16_t addr, uint16_t subAddr, uint16_t index, uint8_t type, uint8_t count, const QByteArray& data) :
			BaseRequest{ REQUEST_WRITE, addr }, m_subAddr(subAddr), m_type(type), m_count(count), m_index(index), m_data(data) {};

		uint16_t m_subAddr;
		uint8_t m_type;
		uint8_t m_count;
		uint16_t m_index;
		QByteArray m_data;

		QByteArray toByteArray() const { return QByteArray((const char*)this, sizeof(ExtendedWriteRequest) - sizeof(QByteArray)) + m_data; }
	};

	struct EncapsulatedRequest
	{
		EncapsulatedRequest() = default;
		EncapsulatedRequest(uint8_t port, uint8_t abNum, const QByteArray& data) :
			m_port(port), m_abNum(abNum), m_sum(0), m_data(data)
		{
			m_len = data.size();

			for (size_t i = 0; i < m_hdrLen - 1; i++)
				m_sum += ((uint8_t*)this)[i];
			m_sum = ~m_sum;
		}

		const uint8_t m_hdrType = REQUEST_ENCAPSULATE;
		const uint8_t m_hdrLen = sizeof(EncapsulatedRequest) - sizeof(QByteArray);
		uint16_t m_len;
		uint8_t m_port;
		uint8_t m_abNum;
		const uint8_t m_rsv = 0x0;
		uint8_t m_sum;
		QByteArray m_data;

		QByteArray toByteArray() const { return QByteArray((const char*)this, sizeof(EncapsulatedRequest) - sizeof(QByteArray)) + m_data; }
	};

	struct BaseResponse
	{
		BaseResponse() : taskNum(0), needRestart(0), isBusy(0), msgType(0), retCode(0) {}
		uint8_t taskNum : 5;
		uint8_t needRestart : 1;
		uint8_t isBusy : 1;
		uint8_t msgType : 1;
		uint8_t retCode;
	};

	struct WriteResponse : BaseResponse
	{
		WriteResponse(const QByteArray buff) {
			memcpy(this, (unsigned char*)buff.data(), sizeof(WriteResponse));
		}

		uint16_t msec = 0;
	};

	struct ReadResponse : BaseResponse
	{
		ReadResponse(const QByteArray& buff) {
			size_t structSize = sizeof(ReadResponse) - sizeof(QByteArray);
			memcpy(this, (unsigned char*)buff.data(), structSize);
			data = QByteArray(buff.data() + structSize, buff.size() - structSize);
		}

		uint8_t type = 0;
		QByteArray data;
	};
#pragma pack(pop)

} // namespace