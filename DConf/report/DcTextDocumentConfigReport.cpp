#include "DcTextDocumentConfigReport.h"

#include <qdebug.h>
#include <qtextdocument.h>
#include <qtexttable.h>
#include <qtextdocumentwriter.h>
#include <qfileinfo.h>

#include <data_model/dc_controller.h>

//=====================================================================================================================================
//	Добавлено 10.05.2023	//	Для обработки вывода в PDF файл
//=====================================================================================================================================
#include <QPrinter>
//=====================================================================================================================================

namespace {
}

DcTextDocumentConfigReport::DcTextDocumentConfigReport(DcController *device) :
	DcIConfigReport(device),
	m_doc(new QTextDocument()),
	m_cursor(new QTextCursor(m_doc))
{
	m_doc->setMetaInformation(QTextDocument::DocumentTitle, QString("Паспорт конфигурации %1").arg(device->name()));
	//m_doc->setPageSize(QSize(842, -1));

	auto blockFormat = cursor()->blockFormat();
	blockFormat.setAlignment(Qt::AlignCenter);
	cursor()->setBlockFormat(blockFormat);

	auto charFormat = cursor()->charFormat();
	charFormat.setFontFamily("Times New Roman");
	charFormat.setFontPointSize(20);
	cursor()->setCharFormat(charFormat);
	cursor()->insertText(QString("Конфигурация устройства"));

	cursor()->insertBlock();
	cursor()->insertBlock();
}

DcTextDocumentConfigReport::~DcTextDocumentConfigReport()
{
	delete m_cursor;
	delete m_doc;
}

void DcTextDocumentConfigReport::nextLine(int count)
{
	//cursor()->setBlockFormat(QTextBlockFormat());

	QTextCharFormat charFormat = cursor()->blockCharFormat();
	charFormat.setFontFamily("Times New Roman");
	charFormat.setFontPointSize(10);
	cursor()->setBlockCharFormat(charFormat);

	for (size_t i = 0; i < count; i++)
		cursor()->insertBlock();
}

void DcTextDocumentConfigReport::insertSection(const QString & section, bool withPrefix, QFont *font)
{
	auto prefixList = prefixs();
	auto sectionName = !section.isEmpty() ? section : !prefixList.isEmpty() ? prefixList.takeLast() : QString();
	if (sectionName.isEmpty())
		return;

	auto blockFormat = cursor()->blockFormat();
	blockFormat.setAlignment(Qt::AlignCenter);
	cursor()->setBlockFormat(blockFormat);

	QFont sectionFont = font ? *font : QFont("Calibri", 13);
	auto charFormat = cursor()->charFormat();
	charFormat.setFont(sectionFont);
	cursor()->setCharFormat(charFormat);
	if (withPrefix) {
		for (auto &it : prefixList)
			cursor()->insertText(QString("%1  »  ").arg(it));
		charFormat.setFontWeight(QFont::Bold);
		cursor()->setCharFormat(charFormat);
	}


	cursor()->insertText(sectionName);
	cursor()->insertBlock();
}

void DcTextDocumentConfigReport::insertDescriptionTable()
{
}

void DcTextDocumentConfigReport::insertTable(const DcReportTable &reportTable)
{
	auto values = reportTable.values();
	auto headers = reportTable.headers();

	if (values.isEmpty() || headers.size() < 2)
		return;

	QVector<QTextLength> constraints;
	for (size_t i = 0; i < headers.size(); i++) {
		int size = reportTable.columnsSize().value(i, 0);
		int percent = size ? size : 100 / headers.size();
		constraints << QTextLength(QTextLength::PercentageLength, percent);
	}

	QTextTableFormat tableFormat;
	tableFormat.setCellSpacing(0);
	tableFormat.setCellPadding(5);
	tableFormat.setHeaderRowCount(1);
	tableFormat.setBorder(1);
	tableFormat.setBorderBrush(QColor("#F0F0F0"));
	tableFormat.setBorderStyle(QTextFrameFormat::BorderStyle_Solid);
	tableFormat.setWidth(QTextLength(QTextLength::PercentageLength, 100));
	tableFormat.setColumnWidthConstraints(constraints);
	tableFormat.setAlignment(Qt::AlignCenter);
	auto table = cursor()->insertTable(values.size() + 1, headers.size(), tableFormat);

	for (size_t i = 0; i < headers.size(); i++) {		
		auto cell = cursor()->currentTable()->cellAt(0, i);
		auto cellFormat = cell.format();
		cellFormat.setBackground(QColor("#E0E0E0"));
		cell.setFormat(cellFormat);

		QTextCursor cellCursor = cell.firstCursorPosition();
		auto format = cellCursor.blockFormat();
		format.setAlignment(Qt::AlignCenter);
		cellCursor.setBlockFormat(format);

		QTextCharFormat f;
		f.setFontFamily("Tahoma");
		f.setFontWeight(QFont::Bold);
		f.setFontPointSize(7);
		f.setVerticalAlignment(QTextCharFormat::AlignMiddle);
		cursor()->insertText(headers.at(i), f);
		cursor()->movePosition(QTextCursor::NextCell);
	}

	auto nameFormat = cursor()->charFormat();
	nameFormat.setFontPointSize(8);
	nameFormat.setFontWeight(QFont::Normal);

	auto nameBlockFormat = cursor()->blockFormat();
	nameBlockFormat.setAlignment(Qt::AlignLeft);

	auto valueFormat = cursor()->charFormat();
	valueFormat.setFontFamily("Tahoma");
	valueFormat.setFontPointSize(8);
	valueFormat.setFontWeight(QFont::Bold);
	valueFormat.setVerticalAlignment(QTextCharFormat::AlignMiddle);

	auto valueBlockFormat = cursor()->blockFormat();
	valueBlockFormat.setAlignment(Qt::AlignCenter);

	for (size_t i = 0; i < values.size(); i++) {
		auto rowValues = values.at(i);
		for (size_t j = 0; j < headers.size(); j++) {
			auto blockFormat = j ? valueBlockFormat : nameBlockFormat;
			cursor()->setBlockFormat(blockFormat);

			auto charFormat = j ? valueFormat : nameFormat;
			cursor()->insertText(rowValues.value(j), charFormat);

			cursor()->movePosition(QTextCursor::NextCell);
		}
	}

	for (auto &&merge : reportTable.mergeList()) {
		auto cell = cursor()->currentTable()->cellAt(merge.row, merge.column);
		QTextCursor cellCursor = cell.firstCursorPosition();
		cellCursor.setBlockFormat(valueBlockFormat);
		table->mergeCells(merge.row, merge.column, merge.numRows, merge.numCols);	
	}

	cursor()->movePosition(QTextCursor::NextBlock);
	nextLine(3);
}

void DcTextDocumentConfigReport::insertImage(const QImage &image, const QString &imageTitle)
{
	auto blockFormat = cursor()->blockFormat();
	blockFormat.setAlignment(Qt::AlignCenter);
	blockFormat.setPageBreakPolicy(QTextFormat::PageBreak_AlwaysBefore);
	cursor()->setBlockFormat(blockFormat);
	cursor()->insertBlock();

	blockFormat.setPageBreakPolicy(QTextFormat::PageBreak_Auto);
	cursor()->setBlockFormat(blockFormat);

	if (!imageTitle.isEmpty()) {
		QFont font("Times New Roman", -1, -1, true);
		auto charFormat = cursor()->charFormat();
		charFormat.setFont(font);
		cursor()->setCharFormat(charFormat);
		cursor()->insertText(imageTitle);
		cursor()->insertBlock();
		cursor()->insertBlock();
	}
	
	QString fileName = QString("%1.svg").arg(imageTitle);
	QUrl Uri = QUrl::fromLocalFile(fileName);
	m_doc->addResource(QTextDocument::ImageResource, Uri, image);
	m_images[fileName] = image;

	QTextImageFormat imageFormat;
	imageFormat.setName(Uri.toString());	
	cursor()->insertImage(imageFormat);
	cursor()->movePosition(QTextCursor::NextBlock);
	nextLine(2);
}

bool DcTextDocumentConfigReport::save(const QString & fileName)
{
	for (auto &it : m_images) {
		auto imageFileName = QString("%1/%2").arg(QFileInfo(fileName).absolutePath()).arg(it.first);
		if (!it.second.save(imageFileName))
			return false;
	}

	QTextDocumentWriter writer(fileName);
	auto extension = QFileInfo(fileName).suffix().toLower();

	//=====================================================================================================================================
	//	Изменено 10.05.2023	//	Для подготовки к выводу в PDF файл
	//=====================================================================================================================================
	//if ("html" == extension)
	//	writer.setFormat("html");
	//if ("odt" == extension)
	//	writer.setFormat("odf");
	//return writer.write(m_doc);
	//=====================================================================================================================================
	if ("html" == extension) {
		writer.setFormat("html");
		return writer.write(m_doc);	}
	if ("odt" == extension) {
		writer.setFormat("odf");
		return writer.write(m_doc);	}

	//=====================================================================================================================================
	//	Добавлено 10.05.2023	//	Обработка вывода в PDF файл
	//=====================================================================================================================================
	if ("pdf" == extension) {
		QPrinter printer(QPrinter::HighResolution);
		printer.setOutputFormat(QPrinter::PdfFormat);
		printer.setOutputFileName(fileName);
		m_doc->print(&printer);
		return true;	}
	//=====================================================================================================================================

	return false;
}

QTextCursor * DcTextDocumentConfigReport::cursor() const
{
	return m_cursor;
}