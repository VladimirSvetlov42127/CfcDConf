#pragma once

#include <qstring.h>

#include <dpc/dpc_global.h>

namespace Dpc::Sybus
{
	class DPC_EXPORT ParamDimension
	{
	public:
		ParamDimension(uint8_t profileCount = 0, uint16_t subProfileCount = 0);

		uint8_t profileCount() const { return m_profileCount; }
		uint16_t subProfileCount() const { return m_subProfileCount; }

		bool is2D() const;
		operator bool() const;

		QString toString() const;

	private:
		uint8_t m_profileCount;
		uint16_t m_subProfileCount;
	};
} // namespace