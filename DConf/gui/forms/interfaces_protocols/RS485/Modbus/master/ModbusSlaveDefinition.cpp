#include "ModbusSlaveDefinition.h"

#include "Common/common_ModbusCommandsHeaders.h"
#include <qdebug.h>
#include <gui/forms/interfaces_protocols/RS485/Modbus/master/Common/common_ExtraFunctions.h>




ModbusSlaveDefinition::ModbusSlaveDefinition(bool isModifyModbus)
	: m_announce{ 0 }	// Обнуляем структуру для нового слейва
{
	setModbusModeModify(isModifyModbus);	// Сохраняем режим
}

ModbusSlaveDefinition::~ModbusSlaveDefinition()
{
}

bool ModbusSlaveDefinition::checkIfAllParamsCorrect()
{
	if (getSlaveFullDefinitionByteArray().isEmpty())
		return false;

	if (mh_commands.isEmpty())	// Если нет ни одной команды
		return true;

	// Если в количестве ничего не указано, значит команд быть не должно!
	if (m_announce.AinsCnt == 0 && m_announce.DinsCnt == 0 && m_announce.CinsCnt == 0
		&& m_announce.rAoutsCnt == 0 && m_announce.rDoutsCnt == 0
		&& m_announce.wAoutsCnt == 0 && m_announce.wDoutsCnt == 0)
		return false;

	for(spCommandDescr_t command: mh_commands)
	{
		spModbusFunc_t function(command->getModbusFunction());
		if (!function)
			return false;

		if(!command->isSupportedFunctionCode(function->getFunctionType()))	// Если неподдерживаемая функция, значит пользовательская
			command->setReqLen(function->getReqLen());	// Обновляем команду данными из функции. Нужно для пользовательской функции, где ReqLen не обновляется

		if (command->getLen() == 0)	// Если вся длина равна нулю, ошибка!
			return false;
		if (command->getReqLen() == 0)	// Если длина запроса (от модбас функции), то ошибка!
			return false;
		if (command->getReqLen() != function->getModbusRequestByteArray().size())
			return false;
		if (command->getAnalizedRspBcnt() > ModbusCommand_ns::ExpectedFirstRspBcnt_Count)
			return false;

	}	

	return true;
}

spCommandDescr_t ModbusSlaveDefinition::createCommandByType(ModbusMaster_ns::channelType_t type)
{
	spCommandDescr_t command(nullptr);	// Если типа такого нет, вернется nullptr

	switch (type)	// В зависимости от типа команды создаем свой класс для команды
	{
	case ModbusMaster_ns::AinsInd:
		command = spCommandDescr_t(new ModbusCommandAins(isModbusModeModify()));
		break;

	case ModbusMaster_ns::DinsInd:
		command = spCommandDescr_t(new ModbusCommandDins(isModbusModeModify()));
		break;

	case ModbusMaster_ns::CinsInd:
		command = spCommandDescr_t(new ModbusCommandCins(isModbusModeModify()));
		break;

	case ModbusMaster_ns::RAoutsInd:

		break;

	case ModbusMaster_ns::RDoutsInd:

		break;

	case ModbusMaster_ns::WAoutsInd:

		break;

	case ModbusMaster_ns::WDoutsInd:

		break;

	default:
		break;
	}

	return command;
}

bool ModbusSlaveDefinition::addCommand(spCommandDescr_t command)
{
	if (!command)
		return false;

	channelType_t type = command->getCommandType();
	key_t key(type, 0);

	auto it = mh_commands.begin();
	while (it != mh_commands.end()) {
		key.second++;
		it = mh_commands.find(key);
	}

	if (!addCommand(key, command))
		return false;

	return true;
}

bool ModbusSlaveDefinition::addCommand(key_t key, spCommandDescr_t command)
{
	if(!command)
		return false;

	mh_commands.insert(key, command);

	return true;
}

spCommandDescr_t ModbusSlaveDefinition::getCommand(key_t key) const
{
	QHash<key_t, spCommandDescr_t>::const_iterator it = mh_commands.find(key);
	if (it == mh_commands.end())
		return nullptr;

	spCommandDescr_t command(*it);	// Ищем и возвращает итератор
	return command;	
}

bool ModbusSlaveDefinition::removeCommand(key_t key)
{
	QHash<key_t, spCommandDescr_t>::const_iterator it = mh_commands.find(key);
	if (it == mh_commands.end())
		return false;	

	mh_commands.erase(it);
	
	//spCommandDescr_t с = mh_commands.take(key);	// Убирает из хеш таблицы такой элемент, но т.к. это умный указатель, не удаляем
	return true;
}

void ModbusSlaveDefinition::removeAllTypeCommands(ModbusMaster_ns::channelType_t type)
{
	int index = 0;
	key_t key(type, index++);
	QHash<key_t, spCommandDescr_t>::const_iterator it = mh_commands.find(key);

	while (it != mh_commands.end()) {
		mh_commands.erase(it);
		key.second = index++;
		it = mh_commands.find(key);
	}

}


bool ModbusSlaveDefinition::addInitCommand(spInitCommand initCommand)
{
	mv_initCommands.append(initCommand);
	return true;
}

bool ModbusSlaveDefinition::removeInitCommand(uint8_t index)
{
	if (index > mv_initCommands.size())
		return false;

	mv_initCommands.remove(index);
	return true;
}

ModbusSlaveDefinition::spInitCommand ModbusSlaveDefinition::getInitCommand(uint8_t index) const
{
	if(index > mv_initCommands.size())
		return nullptr;

	return mv_initCommands[index];
}

const QByteArray & ModbusSlaveDefinition::getSlaveFullDefinitionByteArray()
{
	m_slaveByteArray.clear();

	// Добавляем описание слейва 
	const char * structPointer = (const char*)&m_announce;	// По умолчанию младший байт хранится первым по ходу указателя
	int size = sizeof(m_announce);
	m_slaveByteArray = QByteArray(structPointer, size);	// Сначала описания слейва
	   
	if (!mh_commands.isEmpty()) {
		// Чтобы сохранить порядок команд по времени создания, проходимся по хранилищу вручную
		QHash<key_t, spCommandDescr_t>::const_iterator iter;
		QByteArray allCommandsArray;

		for(ModbusMaster_ns::channelType_t type: ModbusMaster_ns::qv_channelTypes)	// Проходим по всем типам команд
		{
			ModbusMaster_ns::channelType_t searhType;
			switch (type)
			{
			case ModbusMaster_ns::AinsInd:
				searhType = ModbusMaster_ns::AinsInd;
				break;
			case ModbusMaster_ns::DinsInd:
				searhType = ModbusMaster_ns::DinsInd;
				break;
			case ModbusMaster_ns::CinsInd:
				searhType = ModbusMaster_ns::CinsInd;
				break;
			case ModbusMaster_ns::RAoutsInd:
				searhType = ModbusMaster_ns::RAoutsInd;
				break;
			case ModbusMaster_ns::RDoutsInd:
				searhType = ModbusMaster_ns::RDoutsInd;
				break;
			case ModbusMaster_ns::WAoutsInd:
				searhType = ModbusMaster_ns::WAoutsInd;
				break;
			case ModbusMaster_ns::WDoutsInd:
				searhType = ModbusMaster_ns::WDoutsInd;
				break;
			default:
				break;
			}

			key_t key(searhType, 0);	// Проверяем каждый ключ с выбранным типом
			iter = mh_commands.find(key);	// Ищем 
			while (iter != mh_commands.end()) {
				allCommandsArray.append(iter.value()->getCommandDescription());	// Добавляем буфер байт этой команды
				key.second++;
				iter = mh_commands.find(key);
			}
		}

		m_slaveByteArray.append(allCommandsArray);	// Потом описание команд
	}
		
	unsigned char zero = 0;
	// Проверяем, есть ли команды инициализации
	if (!mv_initCommands.isEmpty()) {
		m_slaveByteArray.append(zero);
		for (spInitCommand initCommand : mv_initCommands) {
			m_slaveByteArray.append(initCommand->getInitFullCommandByteArray());
		}
	} else 
		m_slaveByteArray.append(zero);

	m_slaveByteArray.append(zero);	// Ноль для обозначения конца конфигурации
	return m_slaveByteArray;
}

QByteArray ModbusSlaveDefinition::getSlaveHeader() const
{
	const char * structPointer = (const char*)&m_announce;
	int size = sizeof(m_announce);
	QByteArray array(structPointer, size);	// Сначала описания слейва
	return array;
}


uint16_t ModbusSlaveDefinition::getTypeCount(ModbusMaster_ns::channelType_t type) const
{
	uint16_t count = 0;
	switch (type)
	{
	case ModbusMaster_ns::AinsInd:
		count = m_announce.AinsCnt;
		break;
	case ModbusMaster_ns::DinsInd:
		count = m_announce.DinsCnt;
		break;
	case ModbusMaster_ns::CinsInd:
		count = m_announce.CinsCnt;
		break;
	case ModbusMaster_ns::RAoutsInd:
		count = m_announce.rAoutsCnt;
		break;
	case ModbusMaster_ns::RDoutsInd:
		count = m_announce.rDoutsCnt;
		break;
	case ModbusMaster_ns::WAoutsInd:
		count = m_announce.wAoutsCnt;
		break;
	case ModbusMaster_ns::WDoutsInd:
		count = m_announce.wDoutsCnt;
		break;
	default:
		break;
	}
	return count;
}

bool ModbusSlaveDefinition::isModbusModeActive() const
{
	if (m_announce.Mode & (1 << ModbusMaster_ns::ModbusSlaveMode_Active))
		return true;
	else
		return false;
}

bool ModbusSlaveDefinition::isModbusModeModify() const
{
	if (m_announce.Mode & (1 << ModbusMaster_ns::ModbusSlaveNotModbusPDU))
		return true;
	else
		return false;
}

bool ModbusSlaveDefinition::isModbusModeSaveInFile() const
{
	if (m_announce.Mode & (1 << ModbusMaster_ns::ModbusSlaveDef_File))
		return true;
	else
		return false;
}

bool ModbusSlaveDefinition::isCRCBitSet() const
{
	if (m_announce.Mode & (1 << ModbusMaster_ns::ModbusSlaveCrcHiEnd))
		return true;
	else
		return false;
}



bool ModbusSlaveDefinition::setFileName(const QString & name)
{
	if (name.size() > getMaxFileNameByteSize()) 
		return false;
	
	m_fileName = name;

	return true;
}

bool ModbusSlaveDefinition::setCustomFileName(const QString & name)
{
	if (name.size() > getMaxFileNameByteSize())
		return false;

	m_customFileName = name;
	return true;
}

bool ModbusSlaveDefinition::setTypeCount(ModbusMaster_ns::channelType_t type, uint16_t count)
{
	qDebug() << "ДОДЕЛАТЬ ПРОВЕРКУ!!! " << Q_FUNC_INFO;

	switch (type)
	{
	case ModbusMaster_ns::AinsInd:
		m_announce.AinsCnt = count;
		break;
	case ModbusMaster_ns::DinsInd:
		m_announce.DinsCnt = count;
		break;
	case ModbusMaster_ns::CinsInd:
		m_announce.CinsCnt = count;
		break;
	case ModbusMaster_ns::RAoutsInd:
		m_announce.rAoutsCnt = count;
		break;
	case ModbusMaster_ns::RDoutsInd:
		m_announce.rDoutsCnt = count;
		break;
	case ModbusMaster_ns::WAoutsInd:
		m_announce.wAoutsCnt = count;
		break;
	case ModbusMaster_ns::WDoutsInd:
		m_announce.wDoutsCnt = count;
		break;
	default:
		return false;
	}
	return true;
}

// setters
bool ModbusSlaveDefinition::setMode(uint8_t val){
	m_announce.Mode = val;
	return true;
}

void ModbusSlaveDefinition::setModbusModeActive(bool isActive)
{
	if (isActive)
		m_announce.Mode |= 1 << ModbusMaster_ns::ModbusSlaveMode_Active;
	else
		m_announce.Mode &= ~(1 << ModbusMaster_ns::ModbusSlaveMode_Active);
}

void ModbusSlaveDefinition::setModbusModeModify(bool isModify)
{
	if (isModify)
		m_announce.Mode |= 1 << ModbusMaster_ns::ModbusSlaveNotModbusPDU;
	else
		m_announce.Mode &= ~(1 << ModbusMaster_ns::ModbusSlaveNotModbusPDU);
}



void ModbusSlaveDefinition::setModbusModeSaveInFile(bool saveInFile)
{
	if(saveInFile)
		m_announce.Mode |= 1 << ModbusMaster_ns::ModbusSlaveDef_File;
	else 
		m_announce.Mode &= ~(1 << ModbusMaster_ns::ModbusSlaveDef_File);
}

void ModbusSlaveDefinition::setModbusModeCRCBit(bool crc)
{
	if (crc)
		m_announce.Mode |= 1 << ModbusMaster_ns::ModbusSlaveCrcHiEnd;
	else
		m_announce.Mode &= ~(1 << ModbusMaster_ns::ModbusSlaveCrcHiEnd);
}

bool ModbusSlaveDefinition::setTimeout(uint8_t val){
	m_announce.TimeoutMs = val;
	return true;
}

bool ModbusSlaveDefinition::setAddress(uint8_t val){
	m_announce.Address = val;
	return true;
}

bool ModbusSlaveDefinition::setAinsCnt(uint16_t val){
	m_announce.AinsCnt = val;
	return true;
}

bool ModbusSlaveDefinition::setDinsCnt(uint16_t val){
	m_announce.DinsCnt = val;
	return true;
}

bool ModbusSlaveDefinition::setCinsCnt(uint16_t val){
	m_announce.CinsCnt = val;
	return true;
}

bool ModbusSlaveDefinition::setRAoutCnt(uint16_t val){
	m_announce.rAoutsCnt = val;
	return true;
}

bool ModbusSlaveDefinition::setRDoutCnt(uint16_t val){
	m_announce.rDoutsCnt = val;
	return true;
}

bool ModbusSlaveDefinition::setWAoutCnt(uint16_t val){
	m_announce.wAoutsCnt = val;
	return true;
}

bool ModbusSlaveDefinition::setWDoutCnt(uint16_t val){
	m_announce.wDoutsCnt = val;
	return true;
}