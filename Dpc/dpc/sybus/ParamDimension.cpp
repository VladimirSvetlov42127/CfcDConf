#include "ParamDimension.h"

namespace Dpc::Sybus
{
	ParamDimension::ParamDimension(uint8_t profileCount, uint16_t subProfileCount) :
		m_profileCount(profileCount), m_subProfileCount(subProfileCount)
	{
	}

	bool ParamDimension::is2D() const
	{
		return m_profileCount > 1;
	}

	ParamDimension::operator bool() const
	{
		return m_profileCount;
	}

	QString ParamDimension::toString() const
	{
		return is2D() ? QString("2D(%1, %2)").arg(profileCount()).arg(subProfileCount()) : QString::number(subProfileCount());
	}
} // namespace
