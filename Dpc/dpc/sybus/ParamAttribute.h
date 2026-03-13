#pragma once

#include <bitset>
#include <qstring.h>

#include <dpc/dpc_global.h>

namespace Dpc::Sybus
{
	class DPC_EXPORT ParamAttribute
	{
		static constexpr uint16_t MaxFlagsSize = 16;

	public:
		enum Flags {
			WND = 3,	/* Write not defence (not guarantee) */
			LCL,		/* Local Parametr ( no remote service) */
			RST,		/* Reset device after change */
			R,			/* Read enable */
			W,			/* Write enable */
			PN,			/* Network password write protect */
			PM,			/* Minipult password write protect */
			_2D,		/* Two dimention array parametr */
			PS,			/* System Password Write protect*/
			CFG,		/* Configuration parametr	*/
			EXT,		/* Support extended request mode */
			NW,			/* Parameter Name write enable */
			N			/* Parameter have Name*/
		};

		ParamAttribute(uint16_t value = 0);
		ParamAttribute(const QString &value);

		bool operator[](size_t flag) const;
		std::bitset<MaxFlagsSize>::reference operator[](size_t flag);
		operator bool() const;

		uint16_t toValue() const;
		QString toString() const;

		static QString toString(Flags flag);
		static int toFlag(const QString &text);

	private:
		std::bitset<MaxFlagsSize> m_bitset;
	};
} // namespace