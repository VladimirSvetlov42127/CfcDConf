#include "ChannelReply.h"

#include <qdebug.h>

namespace {
	using namespace Dpc::Sybus;

	const QHash <ChannelReply::Error, QString> ERRORS_TEXT = {
		{ ChannelReply::NoError, QString() },
		{ ChannelReply::NotConnected, "Сonnection not established"},
		{ ChannelReply::TransportError, "Transport layer error"},
		{ ChannelReply::InvalidEOF, "Invalid end of packet character"},
		{ ChannelReply::InvalidCRC, "Invalid CRC"},
		{ ChannelReply::InvalidASCII, "Invalid ASCII character"},
		{ ChannelReply::InvalidResponseMsgType, "Invalid response format"},
		{ ChannelReply::UnexpectedParamType, "Undefined parameter type"}		
	};

	const QHash <ChannelReply::ReturnCode, QString> RETURN_CODES_TEXT = {
		{ ChannelReply::Good,					 QString() },
		{ ChannelReply::InvalidOperation,		 "Неверная операция" },
		{ ChannelReply::Busy,					 "Устройство занято" },
		{ ChannelReply::IlgCmd,					 "Неизвестная команда" },
		{ ChannelReply::BadCmd,					 "Неправильный формат команды" },
		{ ChannelReply::BadLenCmd,				 "Неправильная длина команды" },
		{ ChannelReply::BadPar,					 "Неверный параметр" },
		{ ChannelReply::ProtectedCmd,			 "Операция запрещена" },
		{ ChannelReply::BadID,					 "Параметр не поддерживается" },
		{ ChannelReply::InvalidType,			 "Неверный Тип данных" },
		{ ChannelReply::InvalidIndex,			 "Неверный индекс" },
		{ ChannelReply::InvalidCount,			 "Неверное количества или соотношения индекса + количество" },
		{ ChannelReply::Protected,				 "Параметр защищен" },
		{ ChannelReply::InvalidLoginPassword,	 "Неверный логин или пароль" },
		{ ChannelReply::InvalidDevice,			 "Неверное устройство(плата)" },
		{ ChannelReply::InvalidValue,			 "Неверное значение параметра" },
	};
}

namespace Dpc::Sybus
{
	ChannelReply::ChannelReply(const ChannelRequest& request, QObject* parent) :
		QObject(parent),
		m_request(request),
		m_state(Idle),
		m_error(NoError),
		m_retCode(Good),
		m_needRestart(false),
		m_isBusy(false),
		m_delay(0),
		m_dataType(0)
	{
	}
	
	ChannelReply::~ChannelReply()
	{
	}

	ChannelRequest ChannelReply::request() const
	{ 
		return m_request; 
	}
	
	ChannelReply::State ChannelReply::state() const
	{ 
		return m_state; 
	}

	ChannelReply::Error ChannelReply::error() const
	{
		return m_error;
	}

	QString ChannelReply::errorString() const
	{
		return m_errorString;
	}

	ChannelReply::ReturnCode ChannelReply::returnCode() const
	{
		return m_retCode;
	}

	QString ChannelReply::returnCodeString() const
	{
		return ::RETURN_CODES_TEXT.value(m_retCode, "Неизвестный код возврата");
	}

	bool ChannelReply::needRestart() const
	{
		return m_needRestart;
	}

	bool ChannelReply::isBusy() const
	{
		return m_isBusy;
	}

	uint16_t ChannelReply::delay() const
	{
		return m_delay;
	}

	uint8_t ChannelReply::dataType() const
	{
		return m_dataType;
	}

	QByteArray ChannelReply::data() const
	{
		return m_data;
	}
	
	void ChannelReply::setState(ChannelReply::State s) 
	{ 
		m_state = s;
		if (Finished == s) 
			emit finished(); 
	}

	void ChannelReply::setError(Error err, const QString& errStr)
	{
		m_error = err;
		m_errorString = !errStr.isEmpty() ? errStr : ::ERRORS_TEXT.value(err, "Unknown error");
	}
	void ChannelReply::setReturnCode(uint8_t code)
	{
		m_retCode = static_cast<ReturnCode>(code);
	}

	void ChannelReply::setNeedRestart(bool need)
	{
		m_needRestart = need;
	}

	void ChannelReply::setBusy(bool busy)
	{
		m_isBusy = busy;
	}

	void ChannelReply::setDelay(uint16_t d) 
	{ 
		m_delay = d; 
	}

	void ChannelReply::setDataType(uint8_t d) 
	{ 
		m_dataType = d; 
	}	

	void ChannelReply::setData(const QByteArray& d) 
	{ 
		m_data = d; 
	}
} // namespace