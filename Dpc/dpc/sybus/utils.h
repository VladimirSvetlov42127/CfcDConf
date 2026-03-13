#pragma once

#include <qstring.h>

#include <dpc/dpc_global.h>

namespace Dpc::Sybus {

	template<typename T>
	struct Restorer
	{
		T &m_var;
		T m_save;
		Restorer(T &var, T newVal) : m_var(var), m_save(var) { m_var = newVal; }
		~Restorer() { m_var = m_save; }
	};

	template<typename T>
	inline QString toHex(T val, int field = -1)
	{
		if (field < 0)
			field = sizeof(T) * 2;

		return QString("0x%1").arg(QString("%1").arg(val, field, 16, QChar('0')).toUpper());
	}


	DPC_EXPORT uint16_t crc16(uint8_t * ptr, uint32_t size, uint16_t init);
} // namespace