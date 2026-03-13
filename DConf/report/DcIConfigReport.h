#pragma once

#include <qstringlist.h>
#include <qimage.h>

#include <report/DcReportTable.h>

class QFont;

class DcController;

class DcIConfigReport
{
public:
	DcIConfigReport(DcController *device);
	virtual ~DcIConfigReport() {}

	DcController* device() const;

	bool addSectionPrefix(const QString &prefix);
	QString takeSecionPrefix();
	
	virtual void nextLine(int count = 1) = 0;
	virtual void insertSection(const QString &section = QString(), bool withPrefix = true, QFont *font = nullptr) = 0;
	virtual void insertDescriptionTable() = 0;
	virtual void insertTable(const DcReportTable &reportTable) = 0;
	virtual void insertImage(const QImage &image, const QString &imageTitle = QString()) = 0;

	virtual bool save(const QString &fileName) = 0;	

protected:
	QStringList prefixs() const;

private:
	DcController *m_device;
	QStringList m_prefixs;
};