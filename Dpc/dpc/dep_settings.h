#ifndef __DEP_SETTINGS_H__
#define __DEP_SETTINGS_H__

//===================================================================================================================================================
//	Подключение библиотек QT
//===================================================================================================================================================
#include <QMap>
#include <QString>
#include <QVariant>

//===================================================================================================================================================
//	Параметры вывода информационного окна
//===================================================================================================================================================
enum ConnectorType {
	VCOM = 1,
	TCP,
	RS485vTCP,
	RS485vVCOM
};

//	Настройки формы авторизации
const QString login_login = "login_login";
const QString login_password = "login_password";
const QString user_role = "user_role";

//	Настройки параметров соединения
const QString connector_type = "type";
const QString com_port = "com";
const QString com_port_name = "comName";
const QString com_baudrate = "baudrate";
const QString ip_address = "ip";
const QString ip_port = "port";

//--- Старые значения ---
//const QString TypeSetting = "type";
//const QString ComPortSetting = "com";
//const QString ComBaudrate = "baudrate";
//const QString IpAddrSetting = "ip";
//const QString IpPortSetting = "port";


//	Настройки формы About
const QString about_message = "about_message";
const QString about_icon = "about_icon";
const QString about_name = "about_name";
const QString about_version = "about_version";

//	Определение типа
typedef QMap<QString, QVariant> DepSettings;

#endif	//	__DEP_SETTINGS_H__

