#include "pqi_event_model.h"

//===================================================================================================================================================
//	Подключение стандартных библиотек
//===================================================================================================================================================

//===================================================================================================================================================
//	Подключение сторонних библиотек
//===================================================================================================================================================

//===================================================================================================================================================
//	Подключение библиотек QT
//===================================================================================================================================================
#include <QColor>

//===================================================================================================================================================
//	Подключение модулей проекта
//===================================================================================================================================================

//===================================================================================================================================================
//	namespace
//===================================================================================================================================================
namespace {
    const QStringList _params_list = {
	    "Перенапряжение фазное",
	    "Сверхток фазный",
	    "Провал фазного напряжения",
	    "Перенапряжение 3Uo",
	    "Перенапряжение U0",
	    "Перенапряжение U2",
	    "Перенапряжение U1",
	    "Провал напряжения прямой последовательности",
	    "Сверхток 3Io",
	    "Сверхток I0",
	    "Сверхток I2",
	    "Сверхток I1" };
	
    const QStringList _header_names = {
		"№", 
		"Событие ПКЭ", 
		"Время нечувствительности (в 20мс)", 
		"Порог от номинала (б/р)", 
		"Гистерезис от номинала (б/р)" };
} 


//===================================================================================================================================================
//	Конструктор и деструктор класса
//===================================================================================================================================================
PqiEventModel::PqiEventModel(DcController* controller, QObject* parent) : QAbstractTableModel(parent)
{
	_controller = controller;
    auto param = controller->paramsRegistry().parameter(SP_FIO_Porog);
    if (!param)
        return;
    int counter = param->profilesCount();
    for (int i = 0; i < counter; i++){
		PqiItem item;
        item.name = paramsList().value(i);
        int index = 256 * i;
        item.Time = controller->paramsRegistry().element(SP_FIO_Porog, index);
        item.ThresHold = controller->paramsRegistry().element(SP_FIO_Porog, index + 1);
        item.Gisteresis = controller->paramsRegistry().element(SP_FIO_Porog, index + 2);
        _items.append(item);
    }
}

PqiEventModel::~PqiEventModel()
{
}

//===================================================================================================================================================
//	Открытые методы класса
//===================================================================================================================================================
QStringList PqiEventModel::paramsList()
{
	return _params_list;
}

QStringList PqiEventModel::headerNames()
{
	return _header_names;
}


//===================================================================================================================================================
//	Перегружаемые методы класса
//===================================================================================================================================================
int PqiEventModel::rowCount(const QModelIndex& parent) const
{
	return _items.size();
}

int PqiEventModel::columnCount(const QModelIndex& parent) const
{
	return TOTAL_COLUMNS;
}

QVariant PqiEventModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (orientation != Qt::Horizontal || role != Qt::DisplayRole) return QVariant();
    if (section >= NUMBER_COLUMN && section < TOTAL_COLUMNS) return headerNames().value(section);

	return QVariant();
}

Qt::ItemFlags PqiEventModel::flags(const QModelIndex& index) const
{
	if (!index.isValid()) return Qt::NoItemFlags;

	Qt::ItemFlags flag = QAbstractTableModel::flags(index);

	int column = index.column();
	if (column != NAME_COLUMN && column != NUMBER_COLUMN) flag |= Qt::ItemIsEditable;
	QVariant val = index.data(Qt::CheckStateRole);
	if (val == Qt::Checked || val == Qt::Unchecked) flag |= Qt::ItemIsUserCheckable;

	return flag;
}

QVariant PqiEventModel::data(const QModelIndex& index, int role) const
{
	if (!index.isValid()) return QVariant();

	//	Вычисление позиции 
	int column = index.column();
	int row = index.row();
	PqiItem item = _items.at(row);


	//	Выравнивание названия сигналов
	if (role == Qt::TextAlignmentRole) {
		if (column == NAME_COLUMN) return QVariant(Qt::AlignLeft | Qt::AlignVCenter);
		return Qt::AlignCenter;	}

	if (role == Qt::EditRole || role == Qt::DisplayRole) {
		if (column == NUMBER_COLUMN) return row + 1;
        if (column == NAME_COLUMN) return item.name;
        if (column == TIME_COLUMN) return item.Time->value().toDouble();
        if (column == THRESHOLD_COLUMN) return item.ThresHold->value().toDouble();
        if (column == GISTERESIS_COLUMN) return item.Gisteresis->value().toDouble(); }

	return QVariant();
}

bool PqiEventModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
	if (!index.isValid()) return false;

	//	Вычисление позиции 
	int column = index.column();
	if (column != TIME_COLUMN && column != THRESHOLD_COLUMN && column != GISTERESIS_COLUMN) return false;

	int row = index.row();
	if (column == TIME_COLUMN) _items.at(row).Time->updateValue(value.toString());
	if (column == THRESHOLD_COLUMN) _items.at(row).ThresHold->updateValue(value.toString());
	if (column == GISTERESIS_COLUMN) _items.at(row).Gisteresis->updateValue(value.toString());
    emit dataChanged(index, index);

    return true;
}


