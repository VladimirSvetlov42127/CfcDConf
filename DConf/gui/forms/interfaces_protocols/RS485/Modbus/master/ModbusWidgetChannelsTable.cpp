#include "ModbusWidgetChannelsTable.h"

#include <qdebug.h>

#include <dpc/gui/widgets/CheckableHeaderView.h>
#include <dpc/gui/dialogs/msg_box/MsgBox.h>
#include <gui/forms/interfaces_protocols/RS485/Modbus/master/Common/common_ExtraFunctions.h>

using namespace Dpc::Gui;

static const QString STR_COLOMN_NAME("Имя команды");				// Имена заголовков в таблицах
static const QString STR_COLOMN_NUMBER("Номер регистра");
static const QString STR_COLOMN_COUNT("Количество переменных");
static const QString STR_DEFAULT_CHANNEL_NAME("");		// Для формирования канала с номером
static const QString ERR_STR_NOT_IN_RANGE("Ошибка! Значение не входит в допустимый интервал!");

static const int MIN_ROW_COUNT = 1;		// Кол-во строк при создании таблицы
static const int COLOMN_MAX_COUNT = 3;		// Количество колонок в таблицах
static const int COLOMN_NAME = 0;		// Номер колонки в таблице
static const int COLOMN_COUNT = 1;
static const int COLOMN_CHANNEL = 2;



ModbusWidgetChannelsTable::ModbusWidgetChannelsTable(bool isModifyModbus, QWidget *parent)
	: QWidget(parent),
	m_maxChannelsCount(64),
	m_currentModel(nullptr),
	m_isModifyModbus(isModifyModbus)
{
	ui.setupUi(this);
	buildToolBoxTable();
	qRegisterMetaType<ModbusMaster_ns::structCommand_t>("ModbusMaster_ns::structCommand_t");
}

ModbusWidgetChannelsTable::~ModbusWidgetChannelsTable()
{
}

void ModbusWidgetChannelsTable::clearAllForms()
{
	for (pairNameAndModel_t pair : mv_modelVector) {
		ModbusMaster_ns::channelType_t type = pair.first;
		slot_removeTable(type);
	}
	
}

bool ModbusWidgetChannelsTable::buildToolBoxTable()
{
	m_toolBox = new QToolBox;
	ui.vLayout->addWidget(m_toolBox);	// Пустой тулбокс
	this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	bool ok = connect(m_toolBox, SIGNAL(currentChanged(int)), this, SLOT(slot_tableChanged(int)));
	Q_ASSERT(ok);

	return true;
}

void ModbusWidgetChannelsTable::fillModelWithHeaders(spModel_t model)
{
	if (!model)
		return;

	QStandardItem* itemName = new QStandardItem();		// Устанавливаем имена заголовков
	itemName->setCheckable(false);
	itemName->setText(STR_COLOMN_NAME);	
	model->setHorizontalHeaderItem(COLOMN_NAME, itemName);
	   
	QStandardItem* itemCount = new QStandardItem();					// Устанавливаем имена заголовков
	itemCount->setCheckable(false);
	itemCount->setText(STR_COLOMN_COUNT);
	model->setHorizontalHeaderItem(COLOMN_COUNT, itemCount);

	if (!m_isModifyModbus) {	// Если не модифицированный, то показываем номера регистров
		QStandardItem* itemSlaveChannel = new QStandardItem();			// Устанавливаем имена заголовков
		itemSlaveChannel->setCheckable(false);
		itemSlaveChannel->setText(STR_COLOMN_NUMBER);
		model->setHorizontalHeaderItem(COLOMN_CHANNEL, itemSlaveChannel);
	}

	addEmptyRowToModel(model);
}

void ModbusWidgetChannelsTable::addEmptyRowToModel(spModel_t model)
{
	if (!model)
		return;

	QList<QStandardItem*> row;
	for (int colomn = 0; colomn < model->columnCount(); colomn++)
	{
		QStandardItem* item = new QStandardItem();
		if (colomn == COLOMN_NAME) {
			item->setCheckable(true);
			item->setText(STR_DEFAULT_CHANNEL_NAME);			
		}
		else {
			item->setTextAlignment(Qt::AlignCenter);
		}
		row.append(item);
	}
	model->appendRow(row);
}

int ModbusWidgetChannelsTable::checkNewTablePosition(ModbusMaster_ns::channelType_t type)
{
	int posForTable = mv_modelVector.size();
	for (int i = 0; i < mv_modelVector.size(); i++) {
		if (type < mv_modelVector[i].first) {
			posForTable = i;
			break;
		}
	}

	return posForTable;
}

ModbusMaster_ns::channelType_t ModbusWidgetChannelsTable::getCurrentChannelType()
{	
	ModbusMaster_ns::channelType_t channelType;			// Если такого типа нет

	if (m_toolBox->count() > 0) {

		QTableView *tabViewChannels;		// Промежуточный варинт. В нем хранится модель
		tabViewChannels = qobject_cast<QTableView *> (m_toolBox->currentWidget());	// Преобразуем в тип

		QStandardItemModel* model = qobject_cast<QStandardItemModel *>(tabViewChannels->model());

		for (int i = 0; i < mv_modelVector.size(); i++) {			// Ищем какой тип у данной модели
			if (mv_modelVector[i].second == model) {
				channelType = mv_modelVector[i].first;
				break;
			}
		}
	}

	return channelType;
}

int ModbusWidgetChannelsTable::getTableIndexForType(ModbusMaster_ns::channelType_t type)
{
	int remInd = -1;
	for (int i = 0; i < mv_modelVector.size(); i++) {	// Ищем в моделях для каждого типа
		if (type == mv_modelVector[i].first) {
			remInd = i;
			break;
		}
	}

	return remInd;
}


// SLOTS
void ModbusWidgetChannelsTable::slot_addTable(ModbusMaster_ns::channelType_t tableName) {

	// Если нет, создаем новую вкладку с новой таблице и моделью
	QTableView *tabViewChannelsNew = new QTableView;						// Табличное представление модели 
	auto myHeader = new CheckableHeaderView(Qt::Horizontal, tabViewChannelsNew);	// Создается пользовательский хедер с чекбоксом
	myHeader->setEnabledChecking(false);
	tabViewChannelsNew->setHorizontalHeader(myHeader);

	spModel_t model(new QStandardItemModel);
	fillModelWithHeaders(model);
	tabViewChannelsNew->setModel(model.data());

	int newPosition = checkNewTablePosition(tableName);	// Получаем позицию, куда надо вставить новую вкладку
	mv_modelVector.insert(newPosition, qMakePair(tableName, model));	// Вставляем в вектор
	m_toolBox->insertItem(newPosition, tabViewChannelsNew, ModbusMaster_ns::g_channelNames[tableName]);
    m_toolBox->setItemIcon(newPosition, QIcon(":/icons/proj.svg"));
	   
	tabViewChannelsNew->setSelectionBehavior(QAbstractItemView::SelectRows);
	tabViewChannelsNew->setSelectionMode(QAbstractItemView::SingleSelection);
	//tabViewChannelsNew->resizeRowsToContents();
	//tabViewChannelsNew->resizeColumnsToContents();
	tabViewChannelsNew->setColumnWidth(COLOMN_NAME, 350);
	tabViewChannelsNew->setColumnWidth(COLOMN_COUNT, 150);
	if(!m_isModifyModbus)
		tabViewChannelsNew->setColumnWidth(COLOMN_CHANNEL, 100);	// Она будет только в стандартном модбасе

	bool ok = QObject::connect(model.data(), SIGNAL(itemChanged(QStandardItem *)), this, SLOT(slot_changeInTable(QStandardItem *)));
	Q_ASSERT(ok);
	this->setVisible(true);
}

void ModbusWidgetChannelsTable::slot_removeTable(ModbusMaster_ns::channelType_t tableType) {

	int remInd(getTableIndexForType(tableType));
	   
	if (remInd < 0) {
		qDebug() << "Нет такого типа в векторе! Тип = " << tableType;
		return;
	}

	QWidget *removeWidget = m_toolBox->widget(remInd);	// Получаем виджет
	m_toolBox->removeItem(remInd);						// Удаляем виджет из тулбокса
	delete removeWidget;								// Высвобождаем память
	mv_modelVector.remove(remInd);						// Удаляем элемент из вектора
	
	if (mv_modelVector.isEmpty()) {	// Если ничего нет для показа
		this->setVisible(false);
		m_currentModel = nullptr;
	}
}

void ModbusWidgetChannelsTable::slot_addExistingCommand(const ModbusMaster_ns::structCommand_t & command) {

	int modelIndex(getTableIndexForType(command.type));	// Ищем, есть ли уже такой тип
	if (modelIndex < 0) {
		return;	// Должно быть создано перед этим вызовом
	}
		
	spModel_t model(mv_modelVector[modelIndex].second);	// Получаем модель с данными данного типа
	
	// Проверяем, является ли последняя строчка пустой
	int rowCount = model->rowCount();
	if (rowCount != 0)
		if (model->item(rowCount - 1, COLOMN_NAME)->text().isEmpty())
			if (model->item(rowCount - 1, COLOMN_COUNT)->text().isEmpty())
				if (!m_isModifyModbus) {	// Если стандартный, то есть это поле
					if (model->item(rowCount - 1, COLOMN_CHANNEL)->text().isEmpty())
						model->removeRow(rowCount - 1);
				} else 
					model->removeRow(rowCount - 1);				

	// Формируем новую строку для модели
	QList<QStandardItem*> row;
	for (int colomn = 0; colomn < model->columnCount(); colomn++)
	{
		QStandardItem* item = new QStandardItem();

		switch (colomn)
		{
		case COLOMN_NAME: {
			item->setCheckable(true);
			if (command.name.isEmpty() /*|| command.name == 0*/)
				item->setText(makeDefaultCommandName(command.index, command.type, command.count, command.startChannel, m_isModifyModbus));
			else
				item->setText(command.name);
			break;
		}
		case COLOMN_COUNT: {
			item->setTextAlignment(Qt::AlignCenter);
			item->setText(QString::number(command.count));
			break;
		}
		case COLOMN_CHANNEL: {	// Отображается только в стандартном модбасе
			item->setTextAlignment(Qt::AlignCenter);
			item->setText(QString::number(command.startChannel));
			break;
		}
		default:
			break;
		}

		row.append(item);
	}
		
	model->insertRow(command.index, row);	// Вставляем на прежнее место
	QTableView *table = static_cast<QTableView *>(m_toolBox->currentWidget());
	table->setCurrentIndex(model->index(command.index, COLOMN_NAME));	// Устанавливаем выделение
}


void ModbusWidgetChannelsTable::slot_removeCommand(const ModbusMaster_ns::structCommand_t &command)
{
	int modelIndex(getTableIndexForType(command.type));	// Ищем, есть ли уже такой тип
	if (modelIndex < 0) {
		return;	// Должно быть создано перед этим вызовом
	}

	spModel_t model(mv_modelVector[modelIndex].second);	// Получаем модель с данными данного типа

	if (!model->removeRow(command.index))
		MsgBox::error("Ошибка удаления строки из таблицы");
	this->repaint();
}

void ModbusWidgetChannelsTable::slot_changeInTable(QStandardItem *item) {
		   	
	if(m_currentModel == nullptr) {
		MsgBox::error("Невозможно получить текущую модель!");
		return;
	}

	int currentRow = item->row();			// Строка текущего измененного итема
	int currentColom = item->column();		// Столбец


	ModbusMaster_ns::channelType_t curTableType = getCurrentChannelType();			// Тип текущей таблицы
	
	QStandardItem *itemCount = m_currentModel->item(currentRow, COLOMN_COUNT);
	QStandardItem *itemName = m_currentModel->item(currentRow, COLOMN_NAME);
	if (!itemCount || !itemName) {
		MsgBox::error("Невозможно получить ячейки из модели!");
		return;
	}

	// Проверяем количество
	bool countOk;
	bool addressOk;
	m_currentModel->blockSignals(true);		// Блокируем сигналы!!! Нельзя выходить досрочно из функции, в конце их включение!  

	ModbusMaster_ns::count_t channelCount(itemCount->text().toUShort(&countOk));
	if (!countOk) {
		itemCount->setText("Введите число");
		m_currentModel->blockSignals(false);
		return;
	}

	ModbusMaster_ns::startAddress_t startChannelAddress = 0;
	if (!m_isModifyModbus) {	// Если стандартный, то рассчитываем
		QStandardItem *itemStartChannel = m_currentModel->item(currentRow, COLOMN_CHANNEL);

		// Проверяем адрес
		startChannelAddress = itemStartChannel->text().toUShort(&addressOk);
		if (!addressOk) {
			itemStartChannel->setText("Введите число");
			m_currentModel->blockSignals(false);
			return;
		}
	}		

	if (itemName->text().isEmpty()) {	// Если имя команды не задано, пишем по умолчанию
		QString name(makeDefaultCommandName(currentRow + 1, curTableType, channelCount, startChannelAddress, m_isModifyModbus));
		itemName->setText(name);
	}
	m_currentModel->blockSignals(false);		// Обязательно не забыть! Но это должно быть перед редактированием модели, иначе строки не добавятся!


	// Меняем имя функции
	if (addressOk && countOk) {	

		ModbusMaster_ns::structCommand_t command{ curTableType , static_cast<uint16_t>(currentRow), startChannelAddress, channelCount, itemName->text() };	// Заполняем команду для отправки

		// Добавление новой строки, если редактировалась последняя строка
		int currentRowCount = m_currentModel->rowCount();
		bool isLastRow = (currentRow == (currentRowCount - 1));
		bool newValueNotEmpty = !item->text().isEmpty();
		bool valueIsValid = (currentColom != COLOMN_NAME) && (item->text() != ERR_STR_NOT_IN_RANGE);

		if (isLastRow && valueIsValid && newValueNotEmpty) {		// Если изменяемая строчка строчка последняя, не имя и не пуста

			if (currentRowCount <= m_maxChannelsCount) {				// Пока кол-во строк умещается в допустимый максимум 			
				addEmptyRowToModel(spModel_t(m_currentModel));							// Добавляем пустую строчку
			}
		}

		emit signal_updateChannelRequest(command);	// Отправляем сигнал на создание или редактирования команды

		//QTableView *table = dynamic_cast<QTableView *>(m_toolBox->currentWidget());
		//if (table) {
		//	table->resizeColumnToContents(COLOMN_NAME);			
		//}
	}	
	
}


void ModbusWidgetChannelsTable::slot_tableChanged(int index) {

	if (index < 0)
		return;
	
	QTableView *table(dynamic_cast<QTableView*> (m_toolBox->widget(index)));
	if (!table) {
		MsgBox::error("ОШИБКА! Невозможно получить текущую таблицу!");
		m_currentModel = nullptr;
		return;
	}

	QStandardItemModel *model = dynamic_cast<QStandardItemModel*>(table->model());
	if (!model) {
		MsgBox::error("ОШИБКА! Невозможно получить текущую модель из таблицы!");
		m_currentModel = nullptr;
		return;
	}

	for (int i = 0; i < mv_modelVector.size(); i++) {			// Ищем какой тип у данной модели
		if (mv_modelVector[i].second.data() == model) {
			m_currentModel = mv_modelVector[i].second;	// Сохраняем умный указатель из вектора!!! Не из модели! Иначе он ее удалит при смене таблицы!
			break;
		}
	}
	//m_currentModel = spModel_t(model);	// ТАК НЕЛЬЗЯ! Удалит при смене окон!

}

void ModbusWidgetChannelsTable::on_butEraseChecked_clicked() {
	if (!m_currentModel)
		return;

	ModbusMaster_ns::channelType_t curTableType = getCurrentChannelType();			// Тип текущей таблицы

	int rows = m_currentModel->rowCount() - 1;
	bool removed = false;

	for (int i = rows; i >= 0 ; i--) {	// Удаляем с конца! 

		QStandardItem * item = m_currentModel->item(i, COLOMN_NAME);
		if (!item)
			continue;

		// Если строка выбрана
		if (item->checkState() == Qt::Checked ) {
			if (!item->text().isEmpty()) {	// Если есть имя команды, значит она уже зарегистрирована
				ModbusMaster_ns::structCommand_t command{ curTableType , ModbusMaster_ns::commandTableIndex_t(i), 0 , 0, "" };	// Заполняем команду для отправки
				emit signal_eraseChannelRequest(command);
			}		
			m_currentModel->removeRow(i);
			removed = true;
		}			
	}

	if (!removed) {	// Если никакой чекбокс не был выбран, удаляем выделенное

		QTableView *table = static_cast<QTableView *> (m_toolBox->currentWidget());
		QModelIndex index = table->currentIndex();	// Выделенная строка.
		QStandardItem * itemName = m_currentModel->item(index.row(), COLOMN_NAME);

		if (!itemName->text().isEmpty()) {	// Если есть имя команды, значит она уже зарегистрирована
			ModbusMaster_ns::structCommand_t command{ curTableType , ModbusMaster_ns::commandTableIndex_t(index.row()), 0 , 0, "" };	// Заполняем команду для отправки
			emit signal_eraseChannelRequest(command);
		}
		m_currentModel->removeRow(index.row());		
	}

}

void ModbusWidgetChannelsTable::on_butAddRow_clicked() {

	if (!m_currentModel)
		return;

	addEmptyRowToModel(m_currentModel);
}

void ModbusWidgetChannelsTable::on_butCheckAll_clicked() {
	if (!m_currentModel)
		return;

	for (int i = 0; i < m_currentModel->rowCount(); i++) {
		m_currentModel->item(i, COLOMN_NAME)->setCheckState(Qt::Checked);
	}
}
