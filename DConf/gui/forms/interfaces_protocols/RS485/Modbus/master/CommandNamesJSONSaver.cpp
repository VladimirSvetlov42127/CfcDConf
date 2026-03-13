#include "CommandNamesJSONSaver.h"
#include <qjsonarray.h>
#include <qjsonobject.h>
#include <qjsondocument.h>

#include <qfile.h>
#include <dpc/gui/dialogs/msg_box/MsgBox.h>
//#include <device_common_files/FileKeeper.h>

using namespace Dpc::Gui;

CommandNamesJSONSaver::CommandNamesJSONSaver()
	
{
}

CommandNamesJSONSaver::~CommandNamesJSONSaver()
{
}

bool CommandNamesJSONSaver::saveCommandNames(uint8_t portNumber, uint8_t slaveInd, const QHash<key_t, spCommandDescr_t>& commands, const QString &filepath)
{
	if (filepath.isEmpty())
		return false;

	QFile keeper(filepath);	// Закрывает в деструкторе
	QJsonArray slaves;	// Массив слейвов, который будет записан в файл
	QJsonObject oneSlave;	// Один слейв

	if (keeper.exists()) {	// Если файл уже существует, берем данные из него
		if (!keeper.open(QIODevice::ReadOnly)) {	// Дописываем в файл
			MsgBox::error("Ошибка открытия файла для сохранения имен функций");
			return false;
		}

		// Считываем что уже есть
		QByteArray saveData = keeper.readAll();
		if (!saveData.isEmpty()) {
			QJsonDocument document(QJsonDocument::fromJson(saveData));
			if (document.isArray()) {
				slaves = document.array();	// Получаем массив с другими слейвами

				auto iter = slaves.begin();
				while (iter != slaves.end()) {
					if (iter->toObject()["port_number"].toInt() == portNumber && iter->toObject()["slave_index"].toInt() == slaveInd) {
						oneSlave = iter->toObject();	// Получаем наш слейв
						slaves.erase(iter);	// Удаляем описание нашего слейва из массива, потому что потом оно дописывается в конце
						break;
					}
					iter++;
				}
			}
			else
				return false;	// Значит парсит файл неправильно!
		}

		keeper.close();	// Закрываем, чтобы открыть и удалить все содержимое
	}
		
	if (!keeper.open(QIODevice::WriteOnly)) {	// Записываем заново, все что считали, хранится в массиве slaves
		MsgBox::error("Ошибка открытия файла для сохранения имен функций");
		return false;
	}

	QJsonArray jarraySlaveCmd;	// Команды слейва

	auto iter = commands.begin();
	while (iter != commands.end()) {

		int type = iter.key().first;
		int index = iter.key().second;

		QString name = iter.value()->getCommandName();

		QJsonObject obj;
		obj["type"] = type;
		obj["index"] = index;
		obj["name"] = name;

		jarraySlaveCmd.append(obj);

		iter++;
	}

	oneSlave["port_number"] = portNumber;
	oneSlave["slave_index"] = slaveInd;
	oneSlave["commands"] = jarraySlaveCmd;
	slaves.append(oneSlave);	// Добавляем слейв в файл

	QJsonDocument document;	// Для записи в файл
	document.setArray(slaves);
	int result = keeper.write(document.toJson());
	if (result < 0)
		return false;

	return true;
}

bool CommandNamesJSONSaver::readCommandNames(uint8_t portNumber, uint8_t slaveInd, spSlave_t slave, const QString &filepath)
{
	if (filepath.isEmpty())
		return false;

	auto hashCommands = slave->getCommandsHash();

	if (hashCommands.isEmpty())
		return false;

	QFile keeper(filepath);	// Закрывает в деструкторе
	if (!keeper.exists()) {
		//MsgBox::error("Файл с именами команд не был найден");
		return false;
	}
	if (!keeper.open(QIODevice::ReadOnly)) {
		//MsgBox::error("Ошибка открытия файла для сохранения имен функций");
		return false;
	}

	QByteArray saveData = keeper.readAll();	// Читаем все

	QJsonDocument document (QJsonDocument::fromJson(saveData));	// Парсим в документ
	if (!document.isArray() || document.isEmpty()) {
		return false;
	}

	QJsonArray allSlaves = document.array();	// Все слейвы всех портов
	QJsonArray commandsArr;	// Команды выбранного слейва
	auto iter = allSlaves.begin();
	while (iter != allSlaves.end()) {
		if (iter->toObject()["port_number"].toInt() == portNumber && iter->toObject()["slave_index"].toInt() == slaveInd) {
			commandsArr = iter->toObject()["commands"].toArray();
			break;
		}

		iter++;
	}
	
	auto iterJSONCmd = commandsArr.begin();	// Начинаем с начала данных документа JSON
	while (iterJSONCmd != commandsArr.end()) {
		QJsonObject obj = iterJSONCmd->toObject();	// Получаем первую запись
		key_t key;	// Формируем ключ
		key.first = static_cast<ModbusMaster_ns::channelType_t> (obj["type"].toInt());
		key.second = obj["index"].toInt();

		auto iterCmd = hashCommands.find(key);	// Ищем, есть ли такая команда в слейве
		if (iterCmd != hashCommands.end()) {
			iterCmd->data()->setCommandName(obj["name"].toString());	// Не проверяем, если не установится, то будет пустая строка, которая будет изменена по умолчанию			
		}

		iterJSONCmd++;
	}

	return true;
}
