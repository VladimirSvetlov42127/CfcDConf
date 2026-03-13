#include "dins_model.h"

//===================================================================================================================================================
//	Подключение стандартных библиотек
//===================================================================================================================================================

//===================================================================================================================================================
//	Подключение сторонних библиотек
//===================================================================================================================================================
#include <dpc/gui/widgets/TableView.h>

//===================================================================================================================================================
//	Подключение библиотек QT
//===================================================================================================================================================

//===================================================================================================================================================
//	Подключение модулей проекта
//===================================================================================================================================================


//===================================================================================================================================================
//	Конструктор и деструктор класса
//===================================================================================================================================================
DinsModel::DinsModel(DcController* controller, DcBoard* board, QObject* parent) : QAbstractTableModel(parent)
{
	_controller = controller;
	_board = board;
}

DinsModel::~DinsModel()
{
}

//===================================================================================================================================================
//	Перегружаемые методы класса
//===================================================================================================================================================
int DinsModel::rowCount(const QModelIndex& parent) const
{
	if (parent.isValid()) return 0;
	return _items.count();
}

int DinsModel::columnCount(const QModelIndex& parent) const
{
	if (parent.isValid()) return 0;
	return ColumnsCount;
}

QVariant DinsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (orientation != Qt::Horizontal || role != Qt::DisplayRole) return QVariant();

	switch (section) {
	case Columns::NumberColumn: return "№";
	case Columns::NameColumn: return "Название";
	case Columns::OscillColumn: return "Осциллографирование";
	case Columns::JournalColumn: return "Журнал";
	case Columns::ArhciveColumn: return "Архив";
	case Columns::DrebezgColumn: return "Постоянная обработки\n дребезга (мс)";
	case Columns::DrebezgRatio: return "Коэффициент обработки\n дребезга";
	case Columns::TimerColumn: return "Сторожевой таймер\n дребезга (мс)";
	case Columns::InterferenceColumn: return "Параметр отстройки\n от помех (мс)";
	case Columns::InversionColumn: return "Инверсия";
	case Columns::SaveColumn: return "Сохраняемый";
	case Columns::Fix1Column: return "Фиксация в 1";
    case Columns::DPColumn: return "DP";
    }

	return QVariant();
}

Qt::ItemFlags DinsModel::flags(const QModelIndex& index) const
{
	Qt::ItemFlags flag = QAbstractTableModel::flags(index);
    if (index.column() < 1)
        return flag;

    if (index.column() == Columns::NameColumn) {
        auto subType = _items[index.row()].signal->subtype();
        if (subType == DEF_SIG_SUBTYPE_VIRTUAL || subType == DEF_SIG_SUBTYPE_REMOTE)
            return flag | Qt::ItemIsEditable;
    }
    else if (index.data().isValid()) {
        flag |= Qt::ItemIsEditable;
    }

    QVariant value = index.data(Qt::CheckStateRole);
    if (value == Qt::Checked || value == Qt::Unchecked) {
        flag |= Qt::ItemIsUserCheckable;
    }

	return flag;
}

QVariant DinsModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

	DinsItem item_data = _items.at(index.row());
	int column = index.column();

    if (column == Columns::NameColumn && role == Qt::TextAlignmentRole) {
        return QVariant(Qt::AlignLeft | Qt::AlignVCenter);
    }

    if (role == Qt::TextAlignmentRole) {
        return Qt::AlignCenter;
    }

	if ((column == Columns::NameColumn || column == Columns::DrebezgColumn || column == Columns::DrebezgRatio ||
        column == Columns::NumberColumn || column == Columns::TimerColumn || column == Columns::InterferenceColumn) &&
        (role == Qt::DisplayRole || role == Qt::EditRole)) {

        return item_data.data.value(column);
    }

	if ((column == Columns::OscillColumn || column == Columns::JournalColumn || column == Columns::ArhciveColumn ||
		column == Columns::InversionColumn || column == Columns::SaveColumn || column == Columns::Fix1Column ||
         column == Columns::DPColumn) && role == Qt::CheckStateRole) {

        return item_data.data.value(column);
    }

	return QVariant();
}

bool DinsModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    DinsItem &item_data = _items[index.row()];
    int column = index.column();

    if (Columns::NameColumn == column) {
        QString newName = value.toString().trimmed();
        if (newName.isEmpty())
            return false;

        item_data.data[column] = newName;
        item_data.signal->updateName(newName);
    }

    if (column == Columns::DrebezgColumn || column == Columns::DrebezgRatio ||
		column == Columns::TimerColumn || column == Columns::InterferenceColumn) {
        item_data.data[column] = value;

		if (column == Columns::DrebezgColumn) { 
			_controller->setValue(SP_DIN_INT_CNT, item_data.subTypeIdx, value); 
            if (_board != nullptr && _board->ToBoard())
                _board->paramsRegistry().element(SP_DIN_INT_CNT, index.row())->updateValue(value.toString());
        }
        if (column == Columns::DrebezgRatio) {
            _controller->setValue(SP_DIN_DEBPARS_DOUBLE, item_data.subTypeIdx, value);
        }
		if (column == Columns::TimerColumn) { 
			_controller->setValue(SP_DIN_INT_WDT, item_data.subTypeIdx, value); 
            if (_board != nullptr && _board->ToBoard())
                _board->paramsRegistry().element(SP_DIN_INT_WDT, index.row())->updateValue(value.toString());
        }
		if (column == Columns::InterferenceColumn) {
			_controller->setValue(SP_DIN_NOISE_WDT, item_data.subTypeIdx, value);
            if (_board != nullptr && _board->ToBoard())
                _board->paramsRegistry().element(SP_DIN_NOISE_WDT, index.row())->updateValue(value.toString());
        }

		emit dataChanged(index, index);
        return true;
    }

	if (column == Columns::OscillColumn || column == Columns::JournalColumn || column == Columns::ArhciveColumn ||
		column == Columns::InversionColumn || column == Columns::SaveColumn || column == Columns::Fix1Column ||
		column == Columns::DPColumn) {
		item_data.data[column] = value.toBool() ? Qt::Checked : Qt::Unchecked;
		int val = value.toBool() ? 1 : 0;
		if (column == Columns::InversionColumn) { 
			_controller->setValue(SP_DIN_INVERS, item_data.subTypeIdx, val); 
            if (_board != nullptr && _board->ToBoard())
                _board->paramsRegistry().element(SP_DIN_INVERS, index.row())->updateValue(QString::number(val));
        }

        if (column == Columns::SaveColumn)	_controller->setBitValue(SP_DIN_VDINSAVED, item_data.subTypeIdx, val);
        if (column == Columns::Fix1Column)	_controller->setBitValue(SP_DIN_VDINFIXED, item_data.subTypeIdx, val);
        if (column == Columns::OscillColumn) _controller->setBitValue(SP_DIN_DINOSCMASK, item_data.signal->internalId(), val);
        if (column == Columns::JournalColumn) _controller->setBitValue(SP_TREND_INITIALISE, item_data.signal->internalId(), val);
        if (column == Columns::ArhciveColumn) _controller->setBitValue(SP_DIN_CASH_REQMASK, item_data.signal->internalId(), val);
        if (column == Columns::DPColumn) _controller->setBitValue(SP_DIN_DPSREPRESENT, item_data.signal->internalId(), val);
		emit dataChanged(index, index);
        return true;
    }

	return false;
}


//===================================================================================================================================================
//  Вспомогательные методы класса
//===================================================================================================================================================
void DinsModel::SetItems(const QList<DinsItem> &items)
{
	beginResetModel();
	_items = items;
	endResetModel();
}
