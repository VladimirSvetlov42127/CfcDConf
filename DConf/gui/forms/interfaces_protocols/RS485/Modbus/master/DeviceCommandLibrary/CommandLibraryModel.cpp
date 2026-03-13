#include "CommandLibraryModel.h"

#include <gui/forms/interfaces_protocols/RS485/Modbus/master/Common/common_ModbusCommandsHeaders.h>
#include <gui/forms/interfaces_protocols/RS485/Modbus/master/Common/common_CommandWidgetHeaders.h>
#include <gui/forms/interfaces_protocols/RS485/Modbus/master/Commands/ModbusCommandInitialization.h>
#include <dpc/gui/dialogs/msg_box/MsgBox.h>

using namespace Dpc::Gui;


namespace {
	const ModbusMaster_ns::startAddress_t startAddress = 0;
	const ModbusMaster_ns::count_t count = 1;
	const uint8_t dataBytes = 3;
	const uint8_t functionCode = 0x08;
	const uint8_t subFunctionCode = 0x11;
	const uint8_t expectedByteCount = 3;
}

CommandLibraryModel::CommandLibraryModel(QObject *parent)
	: QAbstractItemModel(parent)
{
	m_rootItem = new CmdLibTreeItem("Каталог");
	setupModelData(m_rootItem);

}

CommandLibraryModel::~CommandLibraryModel()
{
	delete m_rootItem;
}

QVariant CommandLibraryModel::data(const QModelIndex & index, int role) const
{
	if (!index.isValid())
		return QVariant();
	
	switch (role)
	{
	case Qt::DisplayRole: {
		CmdLibTreeItem *item = static_cast<CmdLibTreeItem*>(index.internalPointer());
		return QVariant(item->data(index.column()));
	}

	case Qt::DecorationRole: {	// Устанавливаем иконки
		CmdLibTreeItem *item = static_cast<CmdLibTreeItem*>(index.internalPointer());
		if (index.column() == 0) {
			if (item->childCount() == 0)
				return QIcon(":/icons/signal_in.svg");
			else 
				return QIcon(":/icons/folder.svg");
		}
	}
	}

	return QVariant();
}

Qt::ItemFlags CommandLibraryModel::flags(const QModelIndex & index) const
{
	if (!index.isValid())
		return Qt::NoItemFlags;

	return QAbstractItemModel::flags(index);
}

QVariant CommandLibraryModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
		return QVariant(m_rootItem->data(section));

	return QVariant();
}

QModelIndex CommandLibraryModel::index(int row, int column, const QModelIndex & parent) const
{
	if (!hasIndex(row, column, parent))
		return QModelIndex();

	CmdLibTreeItem *parentItem;

	if (!parent.isValid())
		parentItem = m_rootItem;
	else
		parentItem = static_cast<CmdLibTreeItem*>(parent.internalPointer());

	CmdLibTreeItem *childItem = parentItem->child(row);
	if (childItem)
		return createIndex(row, column, childItem);
	return QModelIndex();
}

QModelIndex CommandLibraryModel::parent(const QModelIndex & index) const
{
	if (!index.isValid())
		return QModelIndex();

	CmdLibTreeItem *childItem = static_cast<CmdLibTreeItem*>(index.internalPointer());
	CmdLibTreeItem *parentItem = childItem->parentItem();

	if (parentItem == m_rootItem)
		return QModelIndex();

	if(!parentItem)
		return QModelIndex();

	return createIndex(parentItem->row(), 0, parentItem);
}

int CommandLibraryModel::rowCount(const QModelIndex & parent) const
{
	CmdLibTreeItem *parentItem;
	if (parent.column() > 0)
		return 0;

	if (!parent.isValid())
		parentItem = m_rootItem;
	else
		parentItem = static_cast<CmdLibTreeItem*>(parent.internalPointer());

	return parentItem->childCount();
}

int CommandLibraryModel::columnCount(const QModelIndex & parent) const
{
	if (parent.isValid())
		return static_cast<CmdLibTreeItem*>(parent.internalPointer())->columnCount();
	return m_rootItem->columnCount();
}

void CommandLibraryModel::setupModelData(CmdLibTreeItem * parent)
{	
	if (!parent) {
		MsgBox::error("Невозможно добавить каталог в несуществующий корень");
		return;
	}

	setMercury230Catalog(parent);
}

void CommandLibraryModel::setMercury230Catalog(CmdLibTreeItem *parent)
{
	if (!parent) {
		MsgBox::error("Невозможно добавить каталог в несуществующий корень");
		return;
	}
	   
	CmdLibTreeItem *itemDevice = new CmdLibTreeItem("Счетчик Меркурий 230", nullptr, parent);
	parent->appendChild(itemDevice);

	if (!makeInitializationCommands(itemDevice)) {
		MsgBox::error("Ошибка загрузки библиотеки для Меркурия. Команды инициализации не загружены");
	}

	if (!makeMercury230PowerCommands(itemDevice)) {
		MsgBox::error("Ошибка загрузки библиотеки для Меркурия. Команды для считывания мощности не загружены");
	}

	if (!makeMercuryCommands(itemDevice, "Фазное напряжение", QVector<QString>({ { "Напряжение по фазе 1" }, { "Напряжение по фазе 2" }, { "Напряжение по фазе 3" } }),
		QVector<QString>({ { "По фазе 1" }, { "По фазе 2" }, { "По фазе 3" } }), QVector<uint8_t>({ { 0x11 }, { 0x12 }, { 0x13 } }), 0, 0.01)) {
		MsgBox::error("Ошибка загрузки библиотеки для Меркурия. Команды для считывания напряжения не загружены");
	}

	if (!makeMercuryCommands(itemDevice, "Ток", QVector<QString>({ { "Ток по фазе 1" }, { "Ток по фазе 2" }, { "Ток по фазе 3" } }), 
		QVector<QString>({ { "По фазе 1" }, { "По фазе 2" }, { "По фазе 3" } }), QVector<uint8_t>({ { 0x21 }, { 0x22 }, { 0x23 } }), 0, 0.001)) {
		MsgBox::error("Ошибка загрузки библиотеки для Меркурия. Команды для считывания тока не загружены");
	}

	if (!makeMercuryCommands(itemDevice, "Коэффициент мощности", QVector<QString>({ {"Коэффициент мощности по сумме фаз"}, { "Коэффициент мощности по фазе 1" }, { "Коэффициент мощности по фазе 2" }, { "Коэффициент мощности по фазе 3" } }),
		QVector<QString>({ {"По сумме фаз"}, { "По фазе 1" }, { "По фазе 2" }, { "По фазе 3" } }), QVector<uint8_t>({ { 0x30 }, { 0x31 }, { 0x32 }, { 0x33 } }), 0, 0.001)) {
		MsgBox::error("Ошибка загрузки библиотеки для Меркурия. Команды для считывания коэффициента мощности не загружены");
	}

	if (!makeSingleCommandForMercury(itemDevice, "Частота сети", "Частота сети", 0x40, 0, 0.01)) {
		MsgBox::error("Ошибка загрузки библиотеки для Меркурия. Команды для считывания частоты сети не загружены");
	}

	if (!makeMercuryCommands(itemDevice, "Угол между фазными напряжениями", QVector<QString>({ { "Угол между фазными напряжениями 1 и 2 фаз" }, { "Угол между фазными напряжениями 1 и 3 фаз" }, { "Угол между фазными напряжениями 2 и 3 фаз" } }),
		QVector<QString>({ { "Между 1 и 2 фаз" }, { "Между 1 и 3 фаз" }, { "Между 2 и 3 фаз" } }), 
		QVector<uint8_t>({ { 0x51 }, { 0x52 }, { 0x53 } }), 0, 0.01)) {
		MsgBox::error("Ошибка загрузки библиотеки для Меркурия. Команды для считывания угла между фазными напряжениями не загружены");
	}

	//if (!makeMercuryCommands(itemDevice, "Коэф. искажения синусоидальных фазных напряжений", QVector<QString>({ { "Коэф. искажения по фазе 1" }, { "Коэф. искажения по фазе 2" }, { "Коэф. искажения по фазе 3" } }),
	//	QVector<QString>({ { "Коэф. искажения по фазе 1" }, { "Коэф. искажения по фазе 2" }, { "Коэф. искажения по фазе 3" } }),
	//	QVector<uint8_t>({ { 0x61 }, { 0x62 }, { 0x63 } }), 0, 0.01)) {
	//	MsgBox::error("Ошибка загрузки библиотеки для Меркурия. Команды для считывания коэф. искажения синусоидальных фазных напряжений не загружены");
	//}

	//if (!makeSingleCommandForMercury(itemDevice, "Температура внутри корпуса прибора", "Температура внутри корпуса прибора", 0x70, 0, 0)) {
	//	MsgBox::error("Ошибка загрузки библиотеки для Меркурия. Команды для считывания температуры внутри корпуса прибора не загружены");
	//}

}

bool CommandLibraryModel::makeInitializationCommands(CmdLibTreeItem * parent)
{
	// Команды инициализаци
	bool isInit = true;
	CmdLibTreeItem *itemInitCommands = new CmdLibTreeItem(isInit, "Инициализация", nullptr, parent);
	parent->appendChild(itemInitCommands);

	uint8_t expByteCount = 1;
	if (!makeInitCommand(itemInitCommands, "Проверка связи", expByteCount, QByteArray{ 1, 0 }, QByteArray{ 1, 0 }))
		return false;

	if (!makeInitCommand(itemInitCommands, "Авторизация", expByteCount, QByteArray{ 1, 0 }, QByteArray{ 8, 1 }))
		return false;

	return true;
}

bool CommandLibraryModel::makeMercury230PowerCommands(CmdLibTreeItem * parent)
{
	CmdLibTreeItem *itemParamListItem = new CmdLibTreeItem("Мощность", nullptr, parent);
	parent->appendChild(itemParamListItem);

	if (!makeMercuryCommands(itemParamListItem, "Активная мощность (P)", QVector<QString>({ {"Активная мощность (P) по сумме фаз"}, { "Активная мощность (P) по фазе 1" }, { "Активная мощность (P) по фазе 2" }, { "Активная мощность (P) по фазе 3" } }),
		QVector<QString>({ {"По сумме фаз"}, { "По фазе 1" }, { "По фазе 2" }, { "По фазе 3" } }), QVector<uint8_t>({ {0}, { 0x1 }, { 0x2 }, { 0x3 } }), 0, 0.01)) {
		return false;
	}

	if (!makeMercuryCommands(itemParamListItem, "Реактивная мощность (Q)", QVector<QString>({ {"Реактивная мощность (Q) по сумме фаз"}, { "Реактивная мощность (Q) по фазе 1" }, { "Реактивная мощность (Q) по фазе 2" }, { "Реактивная мощность (Q) по фазе 3" } }),
		QVector<QString>({ {"По сумме фаз"}, { "По фазе 1" }, { "По фазе 2" }, { "По фазе 3" } }), QVector<uint8_t>({ {0x4}, { 0x5 }, { 0x6 }, { 0x7 } }), 0, 0.01)) {
		return false;
	}

	if (!makeMercuryCommands(itemParamListItem, "Полная мощность (S)", QVector<QString>({ {"Полная мощность (S) по сумме фаз"}, { "Полная мощность (S) по фазе 1" }, { "Полная мощность (S) по фазе 2" }, { "Полная мощность (S) по фазе 3" } }),
		QVector<QString>({ {"По сумме фаз"}, { "По фазе 1" }, { "По фазе 2" }, { "По фазе 3" } }), QVector<uint8_t>({ {0x8}, { 0x9 }, { 0xA }, { 0xB } }), 0, 0.01)) {
		return false;
	}

	return true;
}



bool CommandLibraryModel::makeMercuryCommands(CmdLibTreeItem * parent, const QString& paramName, const QVector<QString> &v_commandNames, const QVector<QString> &v_itemsNames, const  QVector<uint8_t> v_commandCodes, float k0, float k1)
{
	if(!parent)
		return false;

	uint64_t size = v_commandCodes.size();
	if (size != v_itemsNames.size() || size != v_commandNames.size())
		return false;



	CmdLibTreeItem *itemParam = new CmdLibTreeItem(paramName, nullptr, parent);	// Создаем узел, куда будем класть команды
	parent->appendChild(itemParam);

	for (int i = 0; i < size; i++) {
		ModbusCommandAins *command = new ModbusCommandAins(true);
		command->setCommandName(v_commandNames[i]);
		QByteArray function;
		function.append(functionCode); function.append(subFunctionCode); function.append(v_commandCodes[i]);
		if (command->setValues(startAddress, count, ModbusCommand_ns::oprRead, dataBytes, ModbusWidgetCommandAins::ModbusAinBytesOrder_Mercury3B,
			ModbusWidgetCommandAins::ModbusAinValAlg_Mercury_P, expectedByteCount, QByteArray(), function, k0, k1)) {
			sp_Command sp_command(command);	// Создаем умный указатель	
			CmdLibTreeItem *itemSubParam = new CmdLibTreeItem(v_itemsNames[i], sp_command, itemParam);
			itemParam->appendChild(itemSubParam);
		}
	}

	return true;
}

bool CommandLibraryModel::makeInitCommand(CmdLibTreeItem * parent, const QString& paramName, uint8_t expectedByteCount, const QByteArray & firstRspBytes, const QByteArray & commandBuf)
{
	if (!parent)
		return false;
	
	sp_InitCommand command(new ModbusCommandInitialization);
	if (!command->setExpectedBcnt(expectedByteCount))
		return false;

	if (!command->setExpectedFirstRspByte(firstRspBytes))
		return false;

	if (!command->setInitCmdBuf(commandBuf))
		return false;

	CmdLibTreeItem *itemInitCommand = new CmdLibTreeItem(true, paramName, command, parent);
	parent->appendChild(itemInitCommand);

	return true;
}

bool CommandLibraryModel::makeSingleCommandForMercury(CmdLibTreeItem * parent, const QString& commandName, 
	const QString& treeItemName, uint8_t funcCode, float k0, float k1)
{
	if(!parent)
		return false;

	ModbusCommandAins *command = new ModbusCommandAins(true);
	command->setCommandName(commandName);
	QByteArray function;
	function.append(functionCode); function.append(subFunctionCode); function.append(funcCode);

	if (command->setValues(startAddress, count, ModbusCommand_ns::oprRead, dataBytes, ModbusWidgetCommandAins::ModbusAinBytesOrder_Mercury3B,
		ModbusWidgetCommandAins::ModbusAinValAlg_Mercury_P, expectedByteCount, QByteArray(), function, k0, k1)) {
		sp_Command sp_command(command);	// Создаем умный указатель	
		CmdLibTreeItem *itemSubParam = new CmdLibTreeItem(treeItemName, sp_command, parent);
		parent->appendChild(itemSubParam);
	}

	return true;
}
