#pragma once

#include <qstring.h>
#include <qfileinfo.h>

#include "data_model/dc_controller.h"

// Для конфигурирования некоторых функций устройства, используются конфигурационные файлы. Для каждого функционала файлы имеют разную структуру, 
// количество и записываются в разных конфигурационных параметрах. Для удобства отслеживания путей к этим файлам, введено
// понятие файловый менеджер к функционалу. Они нужны, чтобы автоматически по кофнигурации определять какие файлы необходимо записать в устройство при записи
// или прочитаить из устройства при чтении. Данный файл помогает создавать таки файловые менеджеры. Для каждого функционала где
// используется дополнительные конфигурационые файлы, пишется свой менеджер.

// Абстрактный класс файлового менеджера(обработчика), который выявляет дополнительные файлы для конфигурации, которые нужно записать/прочитать в(из) устройство.
class IFileManager
{
public:
	// Структура с информацией о путях конфигурационого файла. На устройстве и в конфигурации.
	struct ConfigFileInfo
	{
		QString devicePath;
		QString localPath;
	};

	using ConfigFileInfoList = QList<ConfigFileInfo>;

	// deviceDriveName - Имя тома на устройстве
	// deviceDir - Папка на устройстве в томе deviceDriveName, в которой будут храниться файлы
	// localDir - Папка в конфигурации где будут храниться файлы. Если не указана, используется папка как и на устройстве т.е deviceDir
	IFileManager(DcController* device, const QString& deviceDriveName, const QString& deviceDir, const QString &localDir = QString());

	// Возвращает путь к файлу fileName на устройстве
	QString devicePath(const QString& fileName = QString()) const;

	// Возвращает путь к файлу fileName в конфигурации
	QString localPath(const QString& fileName = QString()) const;

	DcController* device() const { return m_device; }
	void setDevice(DcController* device);

	// возвращает контейнер, каждый элемент которого содержит информацию(путь на устройстве и в конфигурации) о конфигурационом файле.
	ConfigFileInfoList fileInfoList() const;	
	
protected:
    ParameterElement* getParam(int32_t addr, uint16_t index) const;

	// Метод должен возвращать список конфигурационных файлов которые нужно записать/прочитать из устройсвта.
	virtual QStringList configFiles() const = 0;

	virtual void onNewDeviceSet() {}

private:
	DcController* m_device;
	QString m_deviceDriveName;
	QString m_deviceDir;
	QString m_localDir;
};

using IFileManagerPtr = std::shared_ptr<IFileManager>;
QList<IFileManagerPtr>& fileManagersList();

template<typename T>
class AutoReg {
public:
	AutoReg() { fileManagersList().append(std::make_shared<T>()); }
};

#define FILE_MANAGER_REGISTER(TYPE) \
namespace { AutoReg<TYPE> __var; };

