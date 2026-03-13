#include "config_write_operation.h"

#include <qdiriterator.h>
#include <qtemporarydir.h>
#include <qdebug.h>

#include <dpc/sybus/channel/Channel.h>
#include <dpc/sybus/utils.h>
#include <utils/qzipwriter_p.h>

#include <file_managers/IFileManager.h>

using namespace Dpc::Sybus;

namespace {
	enum OperationSteps {
		WriteFilesStep = 0,
		WriteParamsStep,
		SaveRestartStep,

		TotalStepsCount
	};

	const uint8_t RESTART_FLAG_VALUE = 1;
}

ConfigWriteOperation::ConfigWriteOperation(DcController * config, QObject * parent) :
    AbstractConfigOperation("Запись конфигурации", config, TotalStepsCount, parent),
	m_tempDir(new QTemporaryDir),
	m_filesCount(1),
	m_currentFile(0)
{
	setCheckSoftIdEnabled(true);
}

ConfigWriteOperation::~ConfigWriteOperation()
{
	delete m_tempDir;
}

QString ConfigWriteOperation::ipAddress() const
{
    return m_ip;
}

void ConfigWriteOperation::onChannelProgress(int state)
{
    double totalProgressPerFile = 100.0 / m_filesCount;
    emitProgress(totalProgressPerFile * m_currentFile + totalProgressPerFile * state / 100.0);
}

bool ConfigWriteOperation::before()
{
	if (!m_tempDir->isValid()) {
		addError(QString("Не удалось создать временную директорию для работы с файлами: %1").arg(m_tempDir->errorString()));
		return false;
	}

	if (!packConfigArchive())
		return false;

	return true;
}

bool ConfigWriteOperation::exec()
{
    if (!checkSoftId()) {
        abort();
        return false;
    }

	if (!setPrepareWorkMode())
		return false;

	if (!prepareRestartFlag())
		return false;

	if (!uploadFiles())
		return false;

	if (!writeParams())
		return false;

	if (!readIpAddress())
		return false;

	if (!checkRestartFlag())
		return false;

	if (!saveAndRestart())
		return false;	

    return true;
}

bool ConfigWriteOperation::packConfigArchive()
{
	addInfo("Упаковка конфигурационного архива...");

	QString fileName = m_tempDir->filePath(QFileInfo(configArchiveDevicePath()).fileName());
	QZipWriter zip(fileName);
	if (zip.status() != QZipWriter::NoError) {
		addError(QString("Не удалось создать файл конфигурационного архива %1: %2").arg(fileName).arg(zip.status()));
		return false;
	}

	zip.setCompressionPolicy(QZipWriter::AutoCompress);
    QDir dir(QFileInfo(config()->path()).absolutePath());
	dir.setFilter(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);
	QDirIterator it(dir, QDirIterator::Subdirectories);
	while (it.hasNext()) {
		auto path = it.next();
		auto fi = it.fileInfo();
		auto relativePath = dir.relativeFilePath(fi.absoluteFilePath());

		if (fi.isDir()) {
			zip.setCreationPermissions(QFile::permissions(path));
			zip.addDirectory(relativePath);
			if (zip.status() != QZipWriter::NoError) {
				addError(QString("Не удалось создать папку %1 в конфигурационном архиве: %2").arg(relativePath).arg(zip.status()));
				return false;
			}
		}
		else if (fi.isFile()) {
			QFile file(path);
			zip.setCreationPermissions(file.permissions());

            // Костыль, временый файл удаляется к началу архивирования
            if(!file.exists())
                continue;

			zip.addFile(relativePath, &file);
			if (zip.status() != QZipWriter::NoError) {
				addError(QString("Не удалось упаковать файл %1 в конфигурационный архив: %2").arg(path).arg(zip.status()));
				return false;
			}
		}
	}

	m_configArchiveFile = fileName;
	return true;
}

bool ConfigWriteOperation::setPrepareWorkMode()
{
	if (!channel()->setDeviceWorkMode(Channel::Prepare)) {
		QString msg = QString("Не удалось перевести устройство в режим подготовки: %1").arg(channel()->errorMsg());
		if (Channel::BadID != channel()->errorCode()) {
			addError(msg);
			return false;
		}

		addInfo(msg);
	}

	return true;
}

bool ConfigWriteOperation::prepareRestartFlag()
{
	addInfo("Подготовка флага перезагрузки...");

	auto item = ParamPack::create(T_BYTE, SP_RESET_DEVICE_FLAG);
	item->appendValue(RESTART_FLAG_VALUE);
	if (!channel()->setParam(item)) {
		addError(QString("Не удалось подготовить флаг перезагрузки: %1").arg(channel()->errorMsg()));
		return false;
	}

	return true;
}

bool ConfigWriteOperation::checkRestartFlag()
{
	addInfo("Проверка флага перезагрузки...");

	auto p = channel()->param(SP_RESET_DEVICE_FLAG);
	if (!p) {
		addError(QString("Не удалось проверить флаг перезагрузки: %1").arg(channel()->errorMsg()));
		return false;
	}

	return p->value<uint8_t>() == RESTART_FLAG_VALUE;
}

bool ConfigWriteOperation::uploadFiles()
{
	std::map<QString, QString> configFiles;
	for (auto& fm : fileManagersList()) {
        fm->setDevice(config());
		for (auto&& fi : fm->fileInfoList())
			configFiles[fi.localPath] = fi.devicePath;
	}
	configFiles[m_configArchiveFile] = configArchiveDevicePath();

	addInfo("Запись конфигурационых файлов...");
	setCurrentStep(WriteFilesStep);
	m_filesCount = configFiles.size();
	m_currentFile = 0;

	QElapsedTimer t;
	t.start();

	FileSystemLoginScope fsls(channel());
	if (!fsls.isValid) {
		addError(QString("Не удалось авторизоваться в файловую систему на устройстве: %1").arg(channel()->errorMsg()));
		return false;
	}

	for (auto &it : configFiles) {
		auto source = it.first;
		auto dest = it.second;

		auto destDir = dest.left(dest.lastIndexOf('/'));
		if (!destDir.isEmpty() && !channel()->mkdirectory(destDir)) {
			addError(QString("Не удалось создать папку %1 на устройстве: %2").arg(destDir).arg(channel()->errorMsg()));
			return false;
		}

		if (!channel()->uploadFile(source, dest)) {
			addError(QString("Не удалось загурзить файл %1 на устройство: %2").arg(source).arg(channel()->errorMsg()));
			return false;
		}

		m_currentFile++;
	}	

	addInfo(QString("Файлы записаны за %1 секунд").arg(t.elapsed() / 1000.0));

	return true;
}

bool ConfigWriteOperation::writeParams()
{
    int totalParamsCount = config()->paramsRegistry().size();
    for (auto &board: config()->boards())
        totalParamsCount += board->paramsRegistry().size();

    if (!totalParamsCount)
        return true;

    addInfo("Запись конфигурационых параметров...");
    setCurrentStep(WriteParamsStep, totalParamsCount);
    int currentProgress = 0;

    // запись основных параметров
    for (const auto& [addr, param]: config()->paramsRegistry()) {
        emitProgress(++currentProgress);
        if (!writeParam(param.get()))
            return false;
    }

    // запись параметров плат
    for (auto &board: config()->boards()) {
        if (!board->paramsRegistry().size())
            continue;

        if (!channel()->login(board->slot())) {
            addError(QString("Не удалось авторизоваться на плате %1(%2 %3): %4").arg(board->slot()).arg(board->type()).arg(board->inst()).arg(channel()->errorMsg()));
            return false;
        }

        for (const auto& [addr, param]: board->paramsRegistry()) {
            emitProgress(++currentProgress);
            if (!writeParam(param.get(), board.get()))
                return false;
        }

        m_writtenBoards << board.get();
    }

    return true;
}

bool ConfigWriteOperation::readIpAddress()
{
    auto p = channel()->param(SP_IP4_ADDR);
    if (!p && channel()->errorCode() != Channel::BadID) {
        addError(QString("Не удалось получить значение Ip-Адреса : %1").arg(channel()->errorMsg()));
        return false;
    }

    if (p)
        m_ip = p->value<QString>();

    return true;
}

bool ConfigWriteOperation::saveAndRestart()
{
	setCurrentStep(SaveRestartStep, m_writtenBoards.size() + 1);
	int currentProgress = 0;
    for (int i = 0; i < m_writtenBoards.size(); i++) {
		auto board = m_writtenBoards.at(i);
        if (!saveConfig(board->slot()))
			return false;

        if (!restart(board->slot()))
			return false;

		emitProgress(++currentProgress);
	}

	if (!saveConfig())
		return false;

	if (!restart())
		return false;

	emitProgress(++currentProgress);
	return true;
}

bool ConfigWriteOperation::writeParam(Parameter *param, DcBoard * board)
{
    // если не установлен атрибут на запись, пропускаем
    if (!param->attributes()[ParamAttribute::W])
        return true;

    // если это специальный параметр, пропускаем
    if (DcController::specialParams().contains(param->addr()))
        return true;

    // КОСТЫЛЬ!!! частный случай, чтобы не менять пароли!
    if (param->addr() == SP_PASSWORDMAN || param->addr() == SP_PASSWORDNET)
        return true;

    for(auto& profile: param->profiles()) {
        auto p = ParamPack::create(param->type(), param->addr(), profile.front()->position());
        for(auto& element: profile)
            p->appendValue(element->value());

        if (auto written = channel()->setParam(p, board ? board->slot() : -1); written != p->count()) {

            // В случаях ошибки от 'устройства' добавляем отчет. Но не прерывать всю операцию
            if (Channel::DeviceErrorType == channel()->errorType()) {
                for(size_t i = written; i < profile.size(); ++i) {
                    auto element = profile.at(i).get();
                    QString msg = QString("Параметр %1[%2]").arg(toHex((uint16_t)element->addr())).arg(element->position());
                    if (board)
                        msg.append(QString(" на плате %1(%2 %3)").arg(board->slot()).arg(board->type()).arg(board->inst()));
                    msg.append(QString(" не записан: %1").arg(channel()->errorMsg()));
                    addReport(msg);
                }

                continue;
            }

            // В остальных случаях возводим ошибку и прерываем запись.
            addError(channel()->errorMsg());
            return false;
        }
    }

    return true;
}
