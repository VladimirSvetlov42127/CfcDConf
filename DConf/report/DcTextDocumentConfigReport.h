#pragma once

#include <map>

#include <report/DcIConfigReport.h>

class QTextDocument;
class QTextCursor;

class DcTextDocumentConfigReport : public DcIConfigReport
{
public:
	DcTextDocumentConfigReport(DcController *device);
	virtual ~DcTextDocumentConfigReport();
	
	virtual void nextLine(int count = 1) override;
	virtual void insertSection(const QString &section = QString(), bool withPrefix = true, QFont *font = nullptr) override;
	virtual void insertDescriptionTable() override;
	virtual void insertTable(const DcReportTable &reportTable) override;
	virtual void insertImage(const QImage &image, const QString &imageTitle = QString()) override;

	virtual bool save(const QString &fileName) override;

private:
	QTextCursor* cursor() const;

private:
	QTextDocument *m_doc;
	QTextCursor *m_cursor;
	std::map<QString, QImage> m_images;
};