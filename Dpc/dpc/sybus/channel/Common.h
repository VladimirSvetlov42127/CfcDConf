#pragma once

#include <qvariant.h>

#include <dpc/dpc_global.h>

//TODO remove this (dublicate dpc/dep_settings.h)
namespace Dpc::Sybus
{
	enum ChannelType {
		VCOM = 1,
		TCP,
		RS485vTCP,
		RS485vVCOM
	};

	using SettingsType = QMap<QString, QVariant>;

	const QString TypeSetting = "type";
	const QString LoginSetting = "login";
	const QString PasswordSetting = "password";
	const QString ComPortSetting = "com";
    const QString ComPortNameSetting = "comName";
	const QString IpAddrSetting = "ip";
	const QString IpPortSetting = "port";
	const QString Rs485PortSetting = "rs485_port";
	const QString Rs485DeviceSetting = "rs485_device";

	inline QString settingsToString(const SettingsType& st, const QString &spliter = ";")
	{
		QString value;
		for (auto it = st.cbegin(); it != st.cend(); it++) {
			if (it != st.begin())
				value.append(QString("%1 ").arg(spliter));
			value.append(QString("%1:%2").arg(it.key()).arg(it.value().toString()));
		}

		return value;
	}
	inline SettingsType settingsFromString(const QString& str, const QString& spliter = ";")
	{
		SettingsType st;
		for (auto& line : str.split(spliter)) {
			auto list = line.split(":");
			st[list.value(0).trimmed()] = list.value(1).trimmed();
		}

		return st;
	}
} // namespace