#pragma once

#include <qhash.h>

class DcController;
class ParameterElement;
class DcBoard;

namespace Text {
	namespace ReportTable {
		const QString Name = "Наименование";
		const QString Value = "Значение";
	}
}

class DcReportTable
{
public:

	struct ParamRecord
	{
        ParamRecord(ParameterElement *param, const QString &name = QString(), bool checkable = false, int bit = -1) :
			m_param(param), m_name(name), m_checkable(checkable), m_bit(bit) {}

		ParamRecord(uint16_t addr, uint16_t index, const QString &name = QString(), bool checkable = false, int bit = -1) :
			m_addr(addr), m_index(index), m_name(name), m_checkable(checkable), m_bit(bit) {}

        ParamRecord(ParameterElement *param, const QHash<QString, QString> dictionary, const QString &name = QString()) :
            m_param(param), m_name(name), m_dictionary(dictionary) {}

		ParamRecord(uint16_t addr, uint16_t index, const QHash<QString, QString> dictionary, const QString &name = QString()) :
            m_addr(addr), m_index(index), m_name(name), m_dictionary(dictionary) {}

        ParameterElement* m_param = nullptr;
		int16_t m_board = -1;
		uint16_t m_addr;
		uint16_t m_index;
		QString m_name;
		QHash<QString, QString> m_dictionary;
		bool m_checkable = false;
		int m_bit = -1;
	};

	struct Merge {
		int row;
		int column;
		int numRows;
		int numCols;
	};

	using ValueList = QList<QStringList>;
	using MergeList = QList<Merge>;

	// Если поле headers будет пустым, то будет установятся дефолтные заголовки (наименование, значение) 
	// Поле размер колонки, задается списком в процентах. Список может быть не полным, все недостающие значения будут вычислены в равных пропорциях.
	DcReportTable(DcController *device, const QStringList &headers = QStringList(), const QList<int> &columnsSize = QList<int>());

	static QString checkedSign(bool checked);

	void clear();
	DcController* device() const;
	QStringList headers() const;
	ValueList values() const;
	MergeList mergeList() const;
	QList<int> columnsSize() const;
	int size() const;

	void addRow(const QStringList &rowValues);
	bool addRow(const ParamRecord &rec);
	bool addRow(const QString &name, const QList<ParamRecord> &records);	
	void addMerge(const Merge &m);

	void remove(int index);

	QStringList& operator[](int index) { return m_values[index]; }

private:
	DcController *m_device;
	QStringList m_headers;
	QList<int> m_columnsSize;
	ValueList m_values;
	MergeList m_merges;
};
