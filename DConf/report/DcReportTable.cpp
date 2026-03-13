#include "DcReportTable.h"

#include <qdebug.h>

#include <data_model/dc_controller.h>

namespace {
	const QString g_UncheckedSign = QChar(0x2610);
	const QString g_CheckedSign = QChar(0x2611);
	
	struct ParsedRecord
	{
		bool isGood = false;
		QString name;
		QString value;
	};

	ParsedRecord parseRecord(DcController *device, const DcReportTable::ParamRecord &rec)
    {
        auto element = rec.m_param;
        if (!element) {
            if (rec.m_board < 0)
                element = device->paramsRegistry().element(rec.m_addr, rec.m_index);
            else {
                auto board = device->board(rec.m_board);
                if (board)
                    element = board->paramsRegistry().element(rec.m_addr, rec.m_index);
            }
        }

        if (!element)
            return ParsedRecord();

        QString name = !rec.m_name.isEmpty() ? rec.m_name : element->name();
        QString value = element->value();
		if (!rec.m_dictionary.isEmpty())
			value = rec.m_dictionary[value];
		else if (rec.m_checkable) {
			uint val = value.toUInt();
			if (rec.m_bit > -1)
				val &= (1 << rec.m_bit);
			value = DcReportTable::checkedSign(val);
		}

		return { true, name, value };
	}
}

DcReportTable::DcReportTable(DcController *device, const QStringList &headers, const QList<int> &columnsSize) :
	m_device(device),
	m_headers(headers)
{
	if (m_headers.isEmpty())
		m_headers << Text::ReportTable::Name << Text::ReportTable::Value;

	int totalUsed = 0;
	int countUsed = 0;
    for (int i = 0; i < m_headers.size(); i++) {
		if (i >= columnsSize.size())
			break;

		auto size = columnsSize.at(i);
		m_columnsSize << size;
		totalUsed += size;
		countUsed++;
	}

	if (m_columnsSize.size() == m_headers.size())
		return;

	auto rest = (100 - totalUsed) / (m_headers.size() - countUsed);
	for (; countUsed < m_headers.size(); countUsed++)
		m_columnsSize << rest;
}

QString DcReportTable::checkedSign(bool checked)
{
	return checked ? g_CheckedSign : g_UncheckedSign;
}

void DcReportTable::clear()
{
	m_values.clear();
}

DcController * DcReportTable::device() const
{
	return m_device;
}

QStringList DcReportTable::headers() const
{
	return m_headers;
}

DcReportTable::ValueList DcReportTable::values() const
{
	return m_values;
}

DcReportTable::MergeList DcReportTable::mergeList() const
{
	return m_merges;
}

QList<int> DcReportTable::columnsSize() const
{
	return m_columnsSize;
}

int DcReportTable::size() const
{
	return m_values.size();
}

void DcReportTable::addRow(const QStringList & rowValues)
{
	m_values << rowValues;
}

bool DcReportTable::addRow(const ParamRecord & rec)
{
	auto pr = parseRecord(device(), rec);
	if (!pr.isGood)
		return false;

	addRow({ pr.name, pr.value });
	return true;
}

bool DcReportTable::addRow(const QString & name, const QList<ParamRecord>& records)
{
	bool hasAnyValue = false;
	QStringList recordsValues;
	for (auto &rec : records) {
		auto pr = parseRecord(device(), rec);
		if (pr.isGood)
			hasAnyValue = true;

		recordsValues << pr.value;
	}

	if (!hasAnyValue)
		return false;

	addRow(QStringList() << name << recordsValues);
	return true;
}

void DcReportTable::addMerge(const Merge & m)
{
	m_merges << m;
}

void DcReportTable::remove(int index)
{
	m_values.removeAt(index);
}
