#pragma once

#include <qstring.h>

#include <dpc/dpc_global.h>

namespace Dpc::Gui
{
	class DPC_EXPORT MsgBox
	{
	public:
		static void info(const QString& msg);
		static void warning(const QString& msg);
		static void error(const QString& msg);
		static bool question(const QString& msg);
	};
} // namespace

