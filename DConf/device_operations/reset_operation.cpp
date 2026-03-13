#include "reset_operation.h"

#include <qdebug.h>

#include <dpc/sybus/channel/Channel.h>

#include <file_managers/IFileManager.h>

#include <device_operations/abstract_config_operation.h>

using namespace Dpc::Sybus;

ResetOperation::ResetOperation(QObject * parent) :
    AbstractOperation("Сброс конфигурации в заводские настройки", 0, parent)
{
}

QString ResetOperation::ipAddress() const
{
	return m_ip;
}

bool ResetOperation::clearFiles()
{
	FileSystemLoginScope fsls(channel());
	if (!fsls.isValid) {
		addError(QString("Не удалось авторизоваться в файловую систему на устройстве: %1").arg(channel()->errorMsg()));
		return false;
	}

    QStringList filesToDelete = { AbstractConfigOperation::configArchiveDevicePath() };
	for (auto &fm: fileManagersList()) {
		auto dir = fm->devicePath();
		auto list = channel()->directoryFileList(dir);
		if (Channel::NoError != channel()->errorType() && Channel::FileNotFound != channel()->errorCode()) {
			addError(QString("Не удалось получить список содержимого папки %1").arg(dir));
			return false;
		}

		for(auto &fileName: list)
            filesToDelete << QString("%1/%2").arg(dir, fileName);
	}

	auto deleteFile = [=](const QString &fileName) {
		if (!channel()->deleteFile(fileName) && channel()->errorCode() != Channel::FileNotFound) {
			addError(QString("Не удалось удалить файл %1 с устройства").arg(fileName));
			return false;
		}

		return true;
	};

	for (auto &fileName : filesToDelete)
		if (!deleteFile(fileName))
			return false;

	return true;
}

bool ResetOperation::exec()
{
	if (!clearFiles())
		return false;

	QHash<int, QString> ipParams;
    if (channel()->type() == TCP)
        ipParams = { {SP_IP4_ADDR, QString()}, {SP_IP4_NETMASK, QString()}, {SP_IP4_GW_ADDR, QString()} };

    auto it = ipParams.cbegin();
    while (it != ipParams.cend()) {
        if (auto p = channel()->param(it.key(), 0); !p) {
            if (channel()->errorCode() != Channel::BadID) {
                addError(QString("Не удалось сохранить Ip-Адреса: %1").arg(channel()->errorMsg()));
                return false;
            }
            else
                it = ipParams.erase(it);
        }
        else {
            ipParams[it.key()] = p->value<QString>();
            it++;
        }
    }

	if (!channel()->resetDevice()) {
		addError(QString("Не удалось сбросить конфигурацию к заводским настройкам: %1").arg(channel()->errorMsg()));
		return false;
	}

    it = ipParams.cbegin();
    while (it != ipParams.cend()) {
		auto p = ParamPack::create(T_IP4ADR, it.key());
		p->appendValue(it.value());
		if (!channel()->setParam(p)) {
			addError(QString("Не удалось востановить Ip-Адреса: %1").arg(channel()->errorMsg()));
			return false;
		}

		it++;
	}

	if (auto p = channel()->param(SP_IP4_ADDR); p)
		m_ip = p->value<QString>();
	else if (channel()->errorCode() != Channel::BadID) {
		addError(QString("Не удалось получить значение Ip-Адреса : %1").arg(channel()->errorMsg()));
		return false;
	}

	if (!channel()->restartDevice(Channel::HardReset)) {
		addError(QString("Не удалось перезагрузить устройство: %1").arg(channel()->errorMsg()));
		return false;
	}

	return true;
}
