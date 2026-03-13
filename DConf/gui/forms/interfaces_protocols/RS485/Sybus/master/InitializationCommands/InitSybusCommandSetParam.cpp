#include "InitSybusCommandSetParam.h"

#include <qmap.h>
#include <dpc/sybus/smparlist.h>

#include <dpc/gui/dialogs/msg_box/MsgBox.h>

using namespace Dpc::Gui;

static const char END_OF_STRING_SYMBOL = '\0';

namespace ns_params_data_types
{
	static const QMap<int, QString> cm_dataTypesNames{
	{T_BYTE, "BYTE"},
	{T_WORD, "WORD"},
	{T_DWORD, "DWORD"},
	{T_FLOAT, "FLOAT"},
	{T_STRING, "STRING"},
	};

	static const QMap<uint8_t, uint8_t> cm_typesByteLen{	// Длина типов данных
	{T_BYTE, 1},
	{T_WORD, 2},
	{T_DWORD, 4},
	{T_FLOAT, 4},
	{T_STRING, 0},	// В каждом параметре разная
	};

}

InitSybusCommandSetParam::InitSybusCommandSetParam(const QByteArray &commandArray, QWidget *parent)
	: InterfaseInitCommand(parent), m_initCommandBuf(commandArray),
	m_len(0), m_address(0), m_code(1), m_count(0), m_index(0), m_type(T_BYTE)
{
	ui.setupUi(this);

	QMapIterator<int, QString> i(ns_params_data_types::cm_dataTypesNames);
	while (i.hasNext()) {
		i.next();
		ui.comBoxParamType->addItem(i.value(), i.key());
	}

	// Если буфер меньше минимального, будем создавать заново
	if (m_initCommandBuf.size() < MIN_COMMAND_LEN) {
		setDefaultValues();
	}
	else {
		if (getConfigFromBuf())	// Читаем из буфера, заполняем форму
			setCustomDataToWidget();	// Показываем данные на форме, в зависимости от типа
	}
			
	ui.lEdCode->setText("0x" + QString::number(ns_sybusInitCommands::SET_PARAM_CODE, ns_sybusInitCommands::PRINT_VIEW));

	bool ok;
	ok = connect(ui.lEdParamNumber, SIGNAL(editingFinished()), SLOT(slot_lEdParamNumber_editingFinished()));
	Q_ASSERT(ok);
	ok = connect(ui.spBoxParamIndex, SIGNAL(valueChanged(int)), SLOT(slot_spBoxParamIndex_valueChanged(int)));
	Q_ASSERT(ok);
	ok = connect(ui.comBoxParamType, SIGNAL(currentIndexChanged(int)), SLOT(slot_comBoxParamType_currentIndexChanged(int)));
	Q_ASSERT(ok);
	ok = connect(ui.spBoxCount, SIGNAL(valueChanged(int)), SLOT(slot_spBoxCount_valueChanged(int)));
	Q_ASSERT(ok);
}

InitSybusCommandSetParam::~InitSybusCommandSetParam()
{
}

QByteArray InitSybusCommandSetParam::getCommandByteArray() const
{
	if (m_count != mv_dataByteArray.size()) {
		MsgBox::error("Количество данных не совпадает!");
		return QByteArray();
	}

	QByteArray fullCommand;
	fullCommand.append(m_len);	// Сам байт, хранящий длину, не учитывается 
	fullCommand.append(m_code);
	fullCommand.append((char*)&m_address, sizeof(m_address));
	fullCommand.append(m_type);
	fullCommand.append(m_count);
	fullCommand.append((char*)&m_index, sizeof(m_index));

	for (QByteArray arr : mv_dataByteArray)
		fullCommand.append(arr);

	return fullCommand;
}


bool InitSybusCommandSetParam::setCustomDataToWidget()
{
	uint16_t lenOfData = m_initCommandBuf.size() - DATA_POS;
	QByteArray dataArr(m_initCommandBuf.right(lenOfData));	// Данные в массиве байт

	uint8_t sizeInBytes = ns_params_data_types::cm_typesByteLen.value(m_type);
	uint16_t byteCount = m_count * sizeInBytes;

	if (dataArr.size() != byteCount && m_type != T_STRING) {
		ui.labInfo->setText("ОШИБКА! Количество данных не сошлось с проверкой.");
		return false;
	}

	switch (m_type)
	{
	case T_BYTE: {
		for (int i = 0; i < m_count; i++) {
			uint8_t oneByte = dataArr[i];
			QByteArray arrByte;	// Хранилище для байта
			arrByte.append(oneByte);
			mv_dataByteArray.append(arrByte);
		}
		break;
	}
	case T_WORD: {
		for (int i = 0; i < byteCount; i += sizeInBytes) {
			QByteArray arr;	// Хранилище
			arr.append(dataArr.mid(i, sizeInBytes));
			mv_dataByteArray.append(arr);
		}
		break;
	}
	case T_DWORD: {
		for (int i = 0; i < byteCount; i += sizeInBytes) {
			QByteArray arr;	// Хранилище
			arr.append(dataArr.mid(i, sizeInBytes));
			mv_dataByteArray.append(arr);
		}
		break;
	}
	case T_FLOAT: {
		for (int i = 0; i < byteCount; i += sizeInBytes) {
			QByteArray arr;	// Хранилище
			arr.append(dataArr.mid(i, sizeInBytes));
			mv_dataByteArray.append(arr);
		}
		break;
	}
	case T_STRING: {

		uint8_t pos = 0;
		uint8_t readedCount = 0;
		for (int i = 0; i < dataArr.size(); i++) {
			if (dataArr.at(i) == END_OF_STRING_SYMBOL) {	// Если встретилась запятая
				readedCount++;

				QByteArray arr;	// Хранилище
				arr.append(dataArr.mid(pos, (i - pos + 1)));
				mv_dataByteArray.append(arr);
				pos = i + 1;
			}
		}

		if (readedCount != m_count) {
			MsgBox::error("Количество считанных строк не совпало с заданным значением!");
			if (readedCount > m_count) {
				while (mv_dataByteArray.size() != m_count) 
					mv_dataByteArray.removeLast();
				
			} else 
				while (mv_dataByteArray.size() != m_count)
				mv_dataByteArray.append(QByteArray(1, END_OF_STRING_SYMBOL));	// Добавляем пустые поля для нераспознанных данных
		}

		break;
	}
	default:
		MsgBox::error("Неизвестный тип параметра");
		break;
	}

	for (int i = 0; i < m_count; i++) { // Заполняем для отображения		
		QLineEdit *edit = new QLineEdit;
		mv_dataFields.append(edit);
		ui.vLayoutForData->addWidget(edit);
	}

	displayData();

	for (int i = 0; i < m_count; i++) {
		connect(mv_dataFields[i], SIGNAL(editingFinished()), SLOT(slot_ParamData_editingFinished()));
	}

	return true;
}

bool InitSybusCommandSetParam::setDefaultValues()
{		 
	// Заполняем данные самой команды	
	m_len = MIN_COMMAND_LEN;	// Записываем в буфер размер
	m_code = ns_sybusInitCommands::SET_PARAM_CODE;	// Записываем в буфер код
	m_type = T_BYTE;
	
	return true;
}

bool InitSybusCommandSetParam::getConfigFromBuf()
{
	if (!checkDefaultValuesFromBuf()) {
		MsgBox::error("При проверке сохраненной команды возникли ошибки. Данные по умолчанию будут перезаписаны");
		setDefaultValues();
		return false;
	}
	
	m_len = m_initCommandBuf.at(LEN_POS);

	uint8_t low = m_initCommandBuf.at(PARAM_NUMBER_POS);
	uint8_t high = m_initCommandBuf.at(PARAM_NUMBER_POS + 1);
	m_address = low + (high << 8);
	QString number = "0x%1";
	number = number.arg(QString::number(m_address, ns_sybusInitCommands::PRINT_VIEW).toUpper(), 4, '0');
	ui.lEdParamNumber->setText(number);

	uint8_t lowInd = m_initCommandBuf.at(PARAM_INDEX_POS);
	uint8_t highInd = m_initCommandBuf.at(PARAM_INDEX_POS + 1);
	uint16_t m_index = lowInd + (highInd << 8);
	ui.spBoxParamIndex->setValue(m_index);

	m_count = m_initCommandBuf.at(COUNT_POS);
	ui.spBoxCount->setValue(m_count);

	m_type = m_initCommandBuf.at(PARAM_TYPE_POS);
	uint8_t typeIndex = ui.comBoxParamType->findData(QVariant(m_type));
	ui.comBoxParamType->setCurrentIndex(typeIndex);
	return true;
}

bool InitSybusCommandSetParam::checkDefaultValuesFromBuf()
{
	if(static_cast<uint8_t>(m_initCommandBuf[CODE_POS]) != ns_sybusInitCommands::SET_PARAM_CODE)
		return false;

	if (m_initCommandBuf[LEN_POS] == 0)
		return false;

	return true;
}

void InitSybusCommandSetParam::changeCountOfDataFields(uint16_t newCount)
{
	// Динамически изменяем появляющиеся поля
	if (newCount > mv_dataFields.size()) {
		while (mv_dataFields.size() != newCount) {	// Добавляем 
			QLineEdit *edit = new QLineEdit;
			mv_dataFields.append(edit);
			ui.vLayoutForData->addWidget(edit);
			connect(edit, SIGNAL(editingFinished()), SLOT(slot_ParamData_editingFinished()));
		}
	}
	else {
		while (mv_dataFields.size() != newCount) {	// Удаляем
			QLineEdit *edit = mv_dataFields.last();
			ui.vLayoutForData->removeWidget(edit);
			mv_dataFields.removeLast();
			edit->setParent(nullptr);
			delete edit;
		}
	}
}

void InitSybusCommandSetParam::displayData()
{
	if (mv_dataFields.count() != mv_dataByteArray.count()) {
		MsgBox::error("Кол-во полей для ввода не совпадает с кол-вом данных!");
	}

	QVector<QString> vecStrVal(mv_dataByteArray.count());

	uint8_t minSize = mv_dataFields.count() > mv_dataByteArray.count() ? mv_dataByteArray.count() : mv_dataFields.count();

	for (int i = 0; i < minSize; i++) {

		QLineEdit * edit = mv_dataFields[i];
		QString str;

		switch (m_type) {
		case T_BYTE: {
			str = QString::number(mv_dataByteArray[i][0]);
			break;
		}
		case T_WORD: {
			uint16_t oneWord = (mv_dataByteArray[i].at(1) << 8) + static_cast<uint8_t>(mv_dataByteArray[i].at(0));
			str = QString::number(oneWord);
			break;
		}
		case T_DWORD: {
			uint32_t oneDWord = static_cast<uint8_t>(mv_dataByteArray[i].at(0)) + (static_cast<uint8_t>(mv_dataByteArray[i].at(1)) << 8) + (static_cast<uint8_t>(mv_dataByteArray[i].at(2)) << 16) + (static_cast<uint8_t>(mv_dataByteArray[i].at(3)) << 24);
			str += QString::number(oneDWord);
			break;
		}
		case T_FLOAT: {
			void *p = mv_dataByteArray[i].data();
			float onefloat = *(static_cast<float*>(p));
			str += QString::number(onefloat);
			break;
		}
		case T_STRING: {
			str = mv_dataByteArray[i];
			break;
		}
		}

		edit->setText(str);
	}
}

void InitSybusCommandSetParam::updateDataInIndex(uint16_t index)
{
	if (mv_dataByteArray.size() != mv_dataFields.size()) {
		MsgBox::error("Количество полей для записи не соответсвует доступным параметрам для записи");
		return;
	}


	QLineEdit *edit = mv_dataFields[index];
	bool ok;
	uint8_t dataSize = ns_params_data_types::cm_typesByteLen.value(m_type);

	switch (m_type)
	{
	case T_BYTE: {
		uint16_t oneByte = edit->text().toUShort(&ok);
		if (!ok || oneByte > 255) {
			edit->setText("Ошибка ввода параметра");
			return;
		}
		mv_dataByteArray[index].clear();
		mv_dataByteArray[index].append(static_cast<uint8_t>(oneByte));
		break;
	}
	case T_WORD: {
				
		uint16_t oneWord = edit->text().toUInt(&ok);
		if (!ok) {
			MsgBox::error("Ошибка ввода параметра");
			return;
		}
		mv_dataByteArray[index].clear();
		mv_dataByteArray[index].append(static_cast<uint8_t>(oneWord & 0xFF));
		mv_dataByteArray[index].append(static_cast<uint8_t>(oneWord >> 8));
		
		break;
	}
	case T_DWORD: {
		
		uint32_t oneDWord = edit->text().toULong(&ok);
		if (!ok) {
			edit->setText("Ошибка ввода параметра");
			return;
		}
		void* p = &oneDWord;
		mv_dataByteArray[index].clear();
		mv_dataByteArray[index].append(static_cast<char*>(p), dataSize);

		break;
	}
	case T_FLOAT: {

		if (sizeof(float) != dataSize) {
			edit->setText("Размер float не равен " + QString::number(dataSize));
			return;
		}

		float oneFloat = edit->text().toFloat(&ok);
		if (!ok) {
			edit->setText("Ошибка ввода параметра");
			return;
		}
		void *pvoid = &oneFloat;
		mv_dataByteArray[index].clear();
		mv_dataByteArray[index].append(static_cast<char*>(pvoid), dataSize);

		break;
	}
	case T_STRING: {
		mv_dataByteArray[index].clear();
		mv_dataByteArray[index].append(edit->text().toUtf8());
		mv_dataByteArray[index].append(END_OF_STRING_SYMBOL);
		break;
	}
	default:
		break;
	}

	m_len = MIN_COMMAND_LEN;
	for(QByteArray arr : mv_dataByteArray)
		m_len += arr.size();


}


// SLOTS

void InitSybusCommandSetParam::slot_lEdParamNumber_editingFinished() {

	bool ok;
	uint16_t paramNumber = ui.lEdParamNumber->text().toInt(&ok, ns_sybusInitCommands::PRINT_VIEW);
	if (!ok) {
		ui.lEdParamNumber->setText("Ошибка формата");
		return;
	}

	m_address = paramNumber;

	QString number = "0x%1";
	number = number.arg(QString::number(paramNumber, ns_sybusInitCommands::PRINT_VIEW).toUpper(), 4, '0');
	ui.lEdParamNumber->setText(number);
}

void InitSybusCommandSetParam::slot_spBoxParamIndex_valueChanged(int value) {
	m_index = value;
}

void InitSybusCommandSetParam::slot_comBoxParamType_currentIndexChanged(int value) {
	m_type = ui.comBoxParamType->currentData().toInt();
	for (int i = 0; i < mv_dataFields.count(); i++)	// Обновляем данные во всех полях
		updateDataInIndex(i);
}

void InitSybusCommandSetParam::slot_spBoxCount_valueChanged(int count) {
	m_count = count;

	changeCountOfDataFields(count);
	   	  	
 	if (m_type != T_STRING) {	// Если строка, обновляем по мере ввода данных
		uint8_t byteSize = ns_params_data_types::cm_typesByteLen.value(m_type);
		uint8_t len = MIN_COMMAND_LEN + byteSize * count;
		m_len = len;
		mv_dataByteArray.resize(count);
	}
	else {

		mv_dataByteArray.resize(count);
		m_len = MIN_COMMAND_LEN;
		for (int i = 0; i < mv_dataFields.size(); i++) {	// Поля должны уже соответствовать новому кол-ву данных
			m_len += mv_dataFields[i]->text().size() + 1; // Прибавляем символ конца строки
			if (mv_dataFields[i]->text().isEmpty()) {	// Если при добавлении новое поле пустое (увеличение размера)
				mv_dataByteArray[i].clear();
				mv_dataByteArray[i].append(END_OF_STRING_SYMBOL);	// То записываем символ конца строки
			}
				
		}
	}	
}

void InitSybusCommandSetParam::slot_lEdParamData_editingFinished() {
	
	//if (ui.lEdParamData->text().isEmpty())
	//	return;

	//uint8_t count = ui.spBoxCount->value();
	//QStringList list(ui.lEdParamData->text().split(','));
	//if (list.count() != count) {
	//	MsgBox::error("Введено неверное количество параметров!");
	//	return;
	//}

	//bool ok;	// для проверки правильности преобразований
	//QByteArray arr;	// Хранение значений в байтовом формате
	//uint8_t type = ui.comBoxParamType->currentData().toInt();
	//uint8_t dataSize = ns_params_data_types::cm_typesByteLen.value(type);

	//switch (type)
	//{
	//case T_BYTE: {
	//	for (int i = 0; i < count; i++) {
	//		uint16_t oneByte = list[i].toUShort(&ok);
	//		if (!ok || oneByte > 255) {
	//			ui.lEdParamData->setText("Ошибка ввода параметра");
	//			arr.clear();
	//			return;
	//		}
	//		arr.append(oneByte);
	//	}
	//	break;
	//}
	//case T_WORD: {

	//	for (int i = 0; i < count; i++) {
	//		uint16_t oneWord = list[i].toUInt(&ok);
	//		if (!ok) {
	//			MsgBox::error("Ошибка ввода параметра");
	//			return;
	//		}
	//		arr.append(oneWord & 0xFF);
	//		arr.append(oneWord >> 8);
	//	}
	//	break;
	//}
	//case T_DWORD: {				

	//	for (int i = 0; i < count; i++) {
	//		uint32_t oneDWord = list[i].toULong(&ok);
	//		if (!ok) {
	//			ui.lEdParamData->setText("Ошибка ввода параметра");
	//			return;
	//		}
	//		void* p = &oneDWord;
	//		arr.append(static_cast<char*>(p), dataSize);
	//	}
	//	break;
	//}
	//case T_FLOAT: {

	//	if (sizeof(float) != dataSize) {
	//		ui.lEdParamData->setText("Размер float не равен " + QString::number(dataSize));
	//		return;
	//	}

	//	for (int i = 0; i < count; i++) {
	//		float oneFloat = list[i].toFloat(&ok);
	//		if (!ok) {
	//			ui.lEdParamData->setText("Ошибка ввода параметра");
	//			return;
	//		}
	//		void *pvoid = &oneFloat;
	//		arr.append(static_cast<char*>(pvoid), dataSize);
	//	}
	//	break;
	//}
	//case T_STRING: {

	//	// Можно обратиться по адресу к контроллеру и узнать длину выбранного параметра и проверять дилну вводимой строки

	//	for (int i = 0; i < count; i++) {
	//		QString str(list[i]);
	//		if (str.at(0) == ' ') {
	//			str.remove(0, 1);	// Удаляем пробелы в начале
	//		}
	//		arr.append(str);
	//		arr.append(END_OF_STRING_SYMBOL);
	//	}
	//	//arr.remove(arr.size() - 1, 1);	// Удаляем последний ноль в конце команды
	//	break;
	//}
	//default:
	//	MsgBox::error("Неизвестный тип параметра, выберите новый");
	//	return;
	//}
	//
	//m_commandArray->resize(MIN_COMMAND_LEN);	// Удаляем старую часть команды
	//m_commandArray->append(arr);	// Добавляем новую
	//(*m_commandArray)[LEN_POS] = m_commandArray->size() - 1;	// Обновляем длину. Первый бай, который эту длину хранит, в расчет не входит! 
}


void InitSybusCommandSetParam::slot_ParamData_editingFinished() {
	QObject *obg = sender();
	QLineEdit *edit = dynamic_cast<QLineEdit*>(obg);
	if (!edit)
		return;

	if (edit->text().isEmpty())
		return;

	int ind = 0;
	for (int i = 0; i < mv_dataFields.size(); i++) {
		if (mv_dataFields[i] == edit) {
			ind = i;
			break;
		}
	}
	if (ind < 0)
		return;

	updateDataInIndex(ind);

}