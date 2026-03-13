#include "gui.h"

namespace Dpc::Gui 
{
	QColor color(ColorRole role)
	{
		switch (role)
		{
		case Dpc::Gui::InformationColor: return QColor("#f2fdff");
		case Dpc::Gui::WarningColor: return QColor("#fffff2");
		case Dpc::Gui::ErrorColor: return QColor("#ffdede");
		case Dpc::Gui::DebugColor: return QColor("#f0f0f0");
		default: return QColor();
		}
	}
}