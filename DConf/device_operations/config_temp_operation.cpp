#include "config_temp_operation.h"

#include <qtemporarydir.h>
#include <qdebug.h>

#include <dpc/sybus/channel/Channel.h>
#include <device_operations/config_read_operation.h>

using namespace Dpc::Sybus;

namespace {
	enum OperationSteps {
		ReadConfigStep = 0,

		TotalStepsCount
	};
}

ConfigTempOperation::ConfigTempOperation(const QString &name, DcController *config, int stepsCount, QObject * parent) :
    AbstractConfigOperation(name, config, stepsCount, parent),
	m_tempDir(new QTemporaryDir),
    m_tempConfig(nullptr)
{
	setCheckSoftIdEnabled(true);
}

ConfigTempOperation::~ConfigTempOperation()
{
    delete m_tempConfig;
	delete m_tempDir;	
}

bool ConfigTempOperation::before()
{
	if (!m_tempDir->isValid()) {
		addError(QString("Не удалось создать временную папку для работы с файлами: %1").arg(m_tempDir->errorString()));
		return false;
	}

    m_tempConfig = config()->clone();
    if (!m_tempConfig) {
		addError("Не удалось склонировать текущий шаблон конфигурации устройства");
		return false;
	}

    m_tempConfig->setPath(m_tempDir->filePath("uconf.db"));
	return true;
}

bool ConfigTempOperation::exec()
{
	if (!readConfig())
		return false;

	return true;
}

bool ConfigTempOperation::readConfig()
{
	addInfo("Вычитывание конфигурации во временную папку...");

    auto readOperation = std::make_shared<ConfigReadOperation>(m_tempConfig, this);
    readOperation->setChannel(channel());
    connect(readOperation.get(), &AbstractOperation::warningMsg, this, &ConfigTempOperation::addWarning);
    connect(readOperation.get(), &AbstractOperation::errorMsg, this, &ConfigTempOperation::addError);
    connect(readOperation.get(), &AbstractOperation::progress, this, &ConfigTempOperation::emitProgress);

	setCurrentStep(ReadConfigStep);
	readOperation->start();
	if (ErrorState == readOperation->state()) {
		return false;
	}

	if (WarningState == readOperation->state()) {
		for (auto &it : readOperation->reportList())
			addReport(it, false);

		if (readOperation->failedFiles().size()) {
			addError(QString("Не удалось вычитать все конфигурационные файлы с устройства"));
			return false;
		}

		for (auto &it : readOperation->failedParams()) {
			auto param = it.first;
			auto board = it.second;
            // Если на устройстве нет каких то параметров, их нужно выкинуть из конфигурации, так как их значения не актуальны.
//			if (board)
//                board->paramsRegistry().remove(param->addr(), param->param());
//			else
//                m_tempConfig->paramsRegistry().remove(param->addr(), param->param());
		}

	}

	return true;
}
