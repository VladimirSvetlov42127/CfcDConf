#pragma once

#include <device_operations/config_temp_operation.h>
#include <gui/forms/DcMenu.h>

class DcIConfigReport;

class ConfigPassportOperation : public ConfigTempOperation
{
	Q_OBJECT

public:
    struct Structure
	{
		DcMenu *section = nullptr;
        QList<Structure> childs;
	};

    ConfigPassportOperation(DcController *config, const Structure &structure, const QString &fileName, QObject *parent = nullptr);

protected:
	virtual bool after() override;

private:
    void fillReport(const Structure &ps, DcIConfigReport *report);

private:
    Structure m_ps;
	QString m_fileName;
	int m_currentSection;
};
