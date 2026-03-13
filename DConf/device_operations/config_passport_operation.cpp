#include "config_passport_operation.h"

#include <qtemporarydir.h>
#include <qdebug.h>
#include <qdesktopservices.h>
#include <qurl.h>

#include <dpc/sybus/channel/Channel.h>
#include <dpc/sybus/utils.h>

#include "report/DcTextDocumentConfigReport.h"

using namespace Dpc::Sybus;

namespace {
	enum OperationSteps {
		CreatePassportStep = 1,
		SaveFileStep,

		TotalStepsCount
	};

    int sectionsCount(const ConfigPassportOperation::Structure &ps) {
		if (!ps.section)
			return 0;

		int count = 1;
		for (auto &it : ps.childs)
			count += sectionsCount(it);

		return count;
	}
}

ConfigPassportOperation::ConfigPassportOperation(DcController * config, const Structure &structure, const QString &fileName, QObject * parent) :
    ConfigTempOperation("Создание паспорта конфигурации", config, TotalStepsCount, parent),
	m_ps(structure),
	m_fileName(fileName),
	m_currentSection(0)
{
}

bool ConfigPassportOperation::after()
{
	addInfo("Создание паспорта конфигурации...");
	setCurrentStep(CreatePassportStep, sectionsCount(m_ps));
	
    auto report = new DcTextDocumentConfigReport(tempConfig());
	m_currentSection = 0;
	fillReport(m_ps, report);

	setCurrentStep(SaveFileStep);
	if (!report->save(m_fileName)) {
		addError(QString("Не удалось сохранить паспорт конфигурации"));
		return false;
	}

	QDesktopServices::openUrl(QUrl(m_fileName));
	return true;
}

void ConfigPassportOperation::fillReport(const Structure &ps, DcIConfigReport *report)
{
	auto title = ps.section->title();
	if (title.isEmpty())
		title = "Информация об устройстве";

	addInfo(QString("Обработка раздела %1").arg(title));
	emitProgress(++m_currentSection);

	if (!ps.section->isAvailableFor(report->device()))
		return;

	auto hasPrefix = report->addSectionPrefix(ps.section->title());

	ps.section->fillReport(report);
	for (auto &it : ps.childs)
		fillReport(it, report);

	if (hasPrefix)
		report->takeSecionPrefix();
}
