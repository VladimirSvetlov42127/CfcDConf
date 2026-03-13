#include "config_read_operation.h"

#include <QDir>
#include <QTemporaryDir>
#include <QDebug>
#include <QDirIterator>

#include <dpc/sybus/channel/Channel.h>
#include <dpc/sybus/utils.h>
#include <utils/qzipreader_p.h>

#include <file_managers/IFileManager.h>

using namespace Dpc::Sybus;

namespace {

enum OperationSteps {
    ReadParamStep = 0,
    ReadFilesStep,
    ReadArchiveStep,
    UnpackArchiveStep,

    TotalStepsCount
};

} // namespace

ConfigReadOperation::ConfigReadOperation(DcController * config, QObject * parent) :
    AbstractConfigOperation("Чтение конфигурации", config, TotalStepsCount, parent),
	m_tempDir(new QTemporaryDir),
	m_filesCount(1),
	m_currentFile(0)
{
    m_tempDir->setAutoRemove(true);
    setCheckSoftIdEnabled(true);
}

ConfigReadOperation::~ConfigReadOperation()
{
	delete m_tempDir;
}

QList<QPair<ParameterElement *, DcBoard *> > ConfigReadOperation::failedParams() const
{
	return m_failedParams;
}

QStringList ConfigReadOperation::failedFiles() const
{
	return m_failedFiles;
}

QHash<int, ParamPackPtr> ConfigReadOperation::specialParams() const
{
    return m_specialParams;
}

void ConfigReadOperation::onChannelProgress(int state)
{
    double totalProgressPerFile = 100.0 / m_filesCount;
    emitProgress(totalProgressPerFile * m_currentFile + totalProgressPerFile * state / 100.0);
}

bool ConfigReadOperation::before()
{
	if (!m_tempDir->isValid()) {
		addError(QString("Не удалось создать временную папку для работы с файлами: %1").arg(m_tempDir->errorString()));
		return false;
	}

    m_tempConfig.reset(config()->clone());
    tempConfig()->setPath(m_tempDir->filePath("uconf.db"));

    return true;
}

bool ConfigReadOperation::exec()
{
    if (!checkSoftId()) {
        abort();
        return false;
    }

	if (!readParams())
		return false;

	if (!readSpecialParams())
		return false;

	{
		QElapsedTimer t;
		t.start();

		FileSystemLoginScope fsls(channel());
		if (!fsls.isValid) {
			addError(QString("Не удалось авторизоваться в файловую систему на устройстве: %1").arg(channel()->errorMsg()));
			return false;
		}

		if (!readFiles())
			return false;

		if (!readConfigArchive())
			return false;

		addInfo(QString("Файлы прочитаны за %1 секунд").arg(t.elapsed() / 1000.0));
	}

	return true;
}

bool ConfigReadOperation::after()
{
    if (!clearDeviceFolder())
        return false;

    if (!clearBindings())
        return false;

    if (!copyFilesToConfig())
        return false;

    if (!unpackConfigArchive())
        return false;

    if (!copyParams())
        return false;

    if (!configureBindings())
        return false;

    return true;
}

DcController *ConfigReadOperation::tempConfig()
{
    return m_tempConfig.get();
}

bool ConfigReadOperation::clearDeviceFolder()
{
	addInfo("Очистка папки устройства...");

    QFileInfo deviceDbFileInfo(config()->path());
	QDir deviceFolder(deviceDbFileInfo.absolutePath());
	for (auto &fi : deviceFolder.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot)) {
		auto path = fi.absoluteFilePath();
		if (fi.isFile() && path == deviceDbFileInfo.absoluteFilePath())
			continue;
		
		if (fi.isDir())
			if (!QDir(path).removeRecursively()) {
				addError(QString("Не удалось удалить полностью папку %1 и содержимое").arg(path));
				return false;
			}

		if (fi.isFile()) {
			QFile file(path);
			if (!file.remove()) {
                addError(QString("Не удалось удалить файл %1: %2").arg(path, file.errorString()));
				return false;
			}
		}
	}

    return true;
}

bool ConfigReadOperation::copyFilesToConfig()
{    
    auto configPath = QFileInfo(config()->path()).absolutePath();
    auto configArchiveFileName = QFileInfo(configArchiveDevicePath()).fileName();
    QDir tempDir(m_tempDir->path());
    QDirIterator it(tempDir.absolutePath(), QDir::AllEntries | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
    while(it.hasNext()) {
        it.next();
        auto fileInfo = it.fileInfo();
        if (fileInfo.fileName() == configArchiveFileName)
            continue;

        auto destPath = QString("%1/%2").arg(configPath, tempDir.relativeFilePath(it.filePath()));
        if (fileInfo.isDir()) {
            if (!QDir(destPath).mkpath(".")) {
                addError(QString("Не удалось создать папку %1").arg(destPath));
                return false;
            }
            continue;
        }

        if (!QFile::copy(fileInfo.absoluteFilePath(), destPath)) {
            addError(QString("Не удалось скопировать файл %1").arg(destPath));
            return false;
        }
    }

    return true;
}

bool ConfigReadOperation::readParams()
{
    int totalParamsCount = tempConfig()->paramsRegistry().size();
    for (auto &board: tempConfig()->boards())
        totalParamsCount += board->paramsRegistry().size();

    if (!totalParamsCount)
        return true;

    addInfo("Чтение конфигурационых параметров...");
    setCurrentStep(ReadParamStep, totalParamsCount);
    int currentProgress = 0;

    // чтение параметров плат
    for (auto &board: tempConfig()->boards()) {
        for (const auto& [addr, param]: board->paramsRegistry()) {
            emitProgress(++currentProgress);
            if (!readParam(param.get(), board.get()))
                return false;
        }
    }

	// чтение основных параметров
    for (const auto& [addr, param]: tempConfig()->paramsRegistry()) {
        emitProgress(++currentProgress);
        if (!readParam(param.get()))
            return false;
    }

	return true;
}

bool ConfigReadOperation::readSpecialParams()
{
	for (auto param : DcController::specialParams(DcController::BaseParam)) {
		auto dim = channel()->dimension(param);
		if (!dim) {
			addError(QString("Не удалось получить размерность параметра %1: %2").arg(toHex(param)).arg(channel()->errorMsg()));
			return false;
		}

		auto dimSize = dim.subProfileCount();
		auto values = channel()->param(param, 0, dimSize);
		if (Channel::NoError != channel()->errorType()) {
			addError(QString("Не удалось получить занчения параметра %1: %2").arg(toHex(param)).arg(channel()->errorMsg()));
			return false;
		}

		m_specialParams[param] = values;
	}

	return true;
}

bool ConfigReadOperation::readFiles()
{
	std::map<QString, QString> configFiles;
	for (auto& fm : fileManagersList()) {
        fm->setDevice(tempConfig());
        for (auto& fi : fm->fileInfoList())
			configFiles[fi.devicePath] = fi.localPath;
	}

	if (configFiles.empty())
		return true;

	addInfo("Чтение конфигурационых файлов...");
	setCurrentStep(ReadFilesStep);
	m_filesCount = configFiles.size();
	m_currentFile = 0;	
		
	for (auto &it: configFiles) {
		auto source = it.first;
		auto dest = it.second;
		auto destFolder = QFileInfo(dest).absolutePath();
		if (!QDir(destFolder).mkpath(".")) {
			addError(QString("Не удалось создать папку %1").arg(destFolder));
			return false;
		}

		if (!channel()->downloadFile(source, dest)) {
            addReport(QString("Не удалось загурзить файл %1 с устройства: %2").arg(source, channel()->errorMsg()));
			m_failedFiles << source;
			//addError(QString("Не удалось загурзить файл %1 с устройства: %2").arg(source).arg(channel()->errorMsg()));
			//return false;
        }

		m_currentFile++;
	}	

	return true;
}

bool ConfigReadOperation::readConfigArchive()
{
	addInfo("Чтение конфигурационного архива...");
	setCurrentStep(ReadArchiveStep);
	m_filesCount = 1;
	m_currentFile = 0;
	m_configArchiveFile.clear();

	QString source = configArchiveDevicePath();
	QString dest = m_tempDir->filePath(QFileInfo(configArchiveDevicePath()).fileName());
	if (!channel()->downloadFile(source, dest) && Channel::FileNotFound != channel()->errorCode()) {
		addError(QString("Не удалось загрузить конфигурационный архив с устройства: %1").arg(channel()->errorMsg()));
		return false;
	}

	if (Channel::NoError == channel()->errorCode())
		m_configArchiveFile = dest;

	if (m_configArchiveFile.isEmpty())
		addInfo("На устройстве нет конфигурационного архива");

	return true;
}

bool ConfigReadOperation::unpackConfigArchive()
{
	if (m_configArchiveFile.isEmpty())
		return true;

	addInfo("Распаковка конфигурационного архива...");

	// Распаковка и копирование нескачаных файлов в проект.
	QZipReader unzip(m_configArchiveFile, QIODevice::ReadOnly);
	if (QZipReader::NoError != unzip.status()) {
		addError(QString("Не удалось распаковать конфигурационный архив: %1").arg(unzip.status()));
		return false;
	}

	auto fileInfoList = unzip.fileInfoList();
	setCurrentStep(UnpackArchiveStep, fileInfoList.size());
	int currentFI = 0;
    QFileInfo deviceFI(config()->path());
	for (auto &fi : fileInfoList) {
		emitProgress(currentFI++);

		if (fi.isDir) {
            QDir dir(QString("%1/%2").arg(deviceFI.absolutePath(), fi.filePath));
			if (dir.exists())
				continue;

			if (!dir.mkpath(".")) {
				addError(QString("Не удалось создать папку %1").arg(dir.absolutePath()));
				return false;
			}
			if (!QFile::setPermissions(dir.absolutePath(), fi.permissions)) {
				addError(QString("Не удалось установить права на папку %1").arg(dir.absolutePath()));
				return false;
			}
		}

		if (fi.isFile) {
			// Определяем предполагаемый путь в папке устройства
            auto destFilePath = QString("%1/%2").arg(deviceFI.absolutePath(), fi.filePath);

            // Если такой файл уже содержится в папке конфигурации устройства, не перезаписываем его.
            if (QFile::exists(destFilePath))
                continue;

            // Все остальные файлы распаковываем в папку конфигурации устройства.
            if (!createFile(destFilePath, unzip.fileData(fi.filePath), fi.permissions))
                return false;
		}
	}

    return true;
}

bool ConfigReadOperation::copyParams()
{
    config()->beginTransaction();
    for(auto &[addr, parameter]: config()->paramsRegistry()) {
        auto tempParameter = tempConfig()->paramsRegistry().parameter(addr);
        for(size_t i = 0; i < parameter->profiles().size(); ++i) {
            auto &profile = parameter->profiles().at(i);
            auto &tempProfile = tempParameter->profiles().at(i);
            for(size_t j = 0; j < profile.size(); ++j)
                profile.at(j)->updateValue(tempProfile.at(j)->value());
        }
    }
    config()->endTransaction();

    return true;
}

bool ConfigReadOperation::clearBindings()
{
    addInfo("Очистка текущих привязок сигналов к сервисам...");
    config()->beginTransaction();
    config()->serviceManager()->clearBindings();
    config()->endTransaction();
    return true;
}

bool ConfigReadOperation::configureBindings()
{
    addInfo("Настройка привязок сигналов к сервисам...");
    config()->beginTransaction();
    auto res = config()->serviceManager()->rebind();
    config()->endTransaction();
    return res;
}

bool ConfigReadOperation::readParam(Parameter *param, DcBoard* board)
{
    // если не установлен атрибут на чтение, пропускаем
    if (!param->attributes()[ParamAttribute::R])
        return true;

    // если param специальный параметр, пропускаем
    if (DcController::specialParams().contains(param->addr()))
        return true;

    for(auto &profile: param->profiles()) {
        auto p = channel()->param(param->addr(), profile.front()->position(), profile.size(), board ? board->slot() : -1);
        if (p) {
            for(size_t i = 0; i < std::min(p->count(), profile.size()); ++i) {
                auto element = profile.at(i).get();
                element->updateValue(p->value<QString>(i));
                element->resetChangedFlag();
            }

            if (p->count() == profile.size())
                continue;
        }

        // В случаях ошибки от 'устройства' добавляем отчет. Но не прерывать всю операцию
        if (Channel::DeviceErrorType == channel()->errorType()) {
            auto startIdx = p ? p->count() : 0;
            for(size_t i = startIdx; i < profile.size(); ++i) {
                auto element = profile.at(i).get();
                m_failedParams << qMakePair(element, board);

                QString msg = QString("Параметр %1[%2]").arg(toHex((uint16_t) element->addr())).arg(element->position());
                if (board)
                    msg.append(QString(" на плате %1(%2 %3)").arg(board->slot()).arg(board->type()).arg(board->inst()));
                msg.append(QString(" не прочитан: %1").arg(channel()->errorMsg()));
                addReport(msg);
            }

            continue;
        }

        addError(channel()->errorMsg());
        return false;
    }

    return true;
}

bool ConfigReadOperation::createFile(const QString & fileName, const QByteArray & data, QFlags<QFileDevice::Permission> permissions)
{
	QFile f(fileName);
	if (!f.open(QIODevice::WriteOnly)) {
        addError(QString("Не удалось создать файл %1: %2").arg(fileName, f.errorString()));
		return false;
	}

	if (f.write(data) < 0) {
        addError(QString("Не удалось записать файл %1: %2").arg(fileName, f.errorString()));
		return false;
	}

	if (!f.setPermissions(permissions)) {
        addError(QString("Не удалос устанвоить права доступа на файл %1: %2").arg(fileName, f.errorString()));
		return false;
	}

	return true;
}
