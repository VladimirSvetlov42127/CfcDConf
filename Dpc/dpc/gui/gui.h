#pragma once

#include <qstring.h>
#include <qicon.h>
#include <qcolor.h>

#include <dpc/dpc_global.h>

namespace Dpc::Gui 
{
	enum ColorRole 
	{
		NoColor,
		InformationColor,
		WarningColor, 
		ErrorColor, 
		DebugColor
	};

	DPC_EXPORT QColor color(ColorRole role);
}