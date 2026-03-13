#include "TabSybusMasterIdentification.h"

#include <QLabel>
#include "qgroupbox.h"
#include "qformlayout.h"

#include "data_model/dc_controller.h"

#include <gui/forms/interfaces_protocols/RS485/Sybus/master/definesSlaveMode.h>
#include <gui/forms/interfaces_protocols/RS485/DcRs485Defines.h>
#include <gui/forms/interfaces_protocols/RS485/macrosCheckPointer.h>
#include <gui/forms/interfaces_protocols/ConnectionDiscretWidget.h>

// Ошибки
#define ERRORTEXT_ERROR				"Ошибка! "	// Проверяется на эту строку! 
#define ERRORTEXT_NOT_IN_RANGE		"Ошибка! Не входит в рамки допустимых значений!"

#define STR_DEFAULT_PARAMETER_IS			"Параметр по умолчанию: "
#define STR_RANGE_OF_VALUES_IS				"Диапазон значений: " 

// Кодируем номера столбцов
#define TABLE_COLOM_PARAMETER_NAME			0		// Кодируем номер столбца
#define TABLE_COLOM_INPUT_OBJECT			1
#define TABLE_COLOM_DESCRIPTION				2
#define TABLE_ROW_NAME_OF_COLOMS			0


// Для слейвов ограничения
#define LIMIT_TIMEOUT_SLAVE_RESPOND_MIN			10
#define LIMIT_TIMEOUT_SLAVE_RESPOND_MAX			100
#define LIMIT_PHYSICAL_ADDRESS_MAX				252
#define LIMIT_XID_LEN_MAX						16		// Согласно Dep_ParmSystem
#define LIMIT_XID_COUNT_MIN						1		// Минимальное количество выбранных параметров для идентификации слейва

#define LIMIT_BYTE_NUMBER						0xff
#define LIMIT_2BYTE_NUMBER						0xffff
#define LIMIT_4BYTE_NUMBER						0xffffffff

// Параметры по умолчанию для слейвов
#define DEFAULT_TIMEOUT_SLAVE_RESPOND			10


#define CODE_TYPE_CODE							0xFC
#define CODE_PROGRAMM_VERSION					0xF8
#define CODE_PHYSICAL_ADDRESS					0x0D
#define CODE_SERIAL_NUMBER						0xFB
#define CODE_PROGRAMM_CODE						0xF1
#define CODE_LOGICAL_ADDRESS					0x05

using namespace Dpc::Gui;


static const QString STR_SLAVE("Ведомый ");
static const QString STR_SUMM_MODE("Суммарный режим");
static const QString STR_PHASE_MODE("Пофазный режим");
static const QString STR_8BIT_COUNTER("8-ми байтовый счетчик");
static const QString STR_4BIT_COUNTER("4-х байтовый счетчик");
static const QString STR_SET_THE_VALUE("Введите значение для активации");
static const QString STR_TYPE_CODE_NONE("Не участвует в идентификации");
  
// Дублируются из общего класса протоколов RS485
const QString ERR_QLINEEDIT_STYLE_SHEET = "QLineEdit { background-color: #ff6b5a; color : black;}";
const QString GOOD_QLINEEDIT_STYLE_SHEET = "QLineEdit { background-color: white }";



// Имена для проверки всех параметров в настройках ведомого

static const QString ERR_STR_WRONG_COUNT_OF_IDENTIF_PARAM("Выбрано недостаточно параметров для идентификации!");
static const QString ERR_STR_SLAVE_NOT_ACTIVE("Не активен!");
static const QString ERR_STR_ERROR("Ошибка! ");
static const QString ERR_STR_TIMEOUT_SUMM_EXCEED("Сумма таймаутов всех ведомых превышает интервал опроса!");

static const QVector<QPair<QString, int>> vectorTypeCodesDefine{ {STR_TYPE_CODE_NONE , 0},
																{"DECONT", 0x01},
																{"DOUT8", 0x3D},
																{"DIN16", 0x3E   },
																{"AIN8", 0x3F   },
																{"R3IN6", 0x40   },
																{"CIN8", 0x41   },
																{"CONSOLE", 0x42   },
																{"R4IN6", 0x43   },
																{"DOUT16", 0x44 },
																{"DOUT16", 0x45 },
																{"AOUT1", 0x46 },
																{"DOUT64tr", 0x47 },
																{"UPS", 0x48 },
																{"SENSOR", 0x49 },
																{"PULT", 0x4B   },
																{"DIN16F", 0x4C },
																{"AIN16", 0x4D   },
																{"DOUT16tr", 0x4F },
																{"AIN8U60", 0x52 },
																{"DIN64", 0x53 },
																{"LED001", 0x51 },
																{"AIN16P", 0x4E },
																{"AIN6I", 0x54 },
																{"R2IN6", 0x55 },
																{"AOUT4", 0x56 },
																{"GPStimer", 0x57 },
																{"Univ", 0x58 },
																{"ExLine", 0x59 },
																{"ALine", 0x5D },
																{"DISPLAY", 0x64   },
																{"Новый стандарт, нет в списке", 150}
																};

TabSybusMasterIdentification::TabSybusMasterIdentification(DcController *controller, int portNumber, QObject *parent)
	: QObject(parent), m_portNumber(portNumber)
{

	m_port_id = m_portNumber * 256;
	
    m_controller = controller;

	widIdentification = new QWidget;
	currentSlaveNumber = 0;

	comBoxChooseSlave = new QComboBox;

	chBoxSlaveStateIsActive = new QCheckBox;
	chBoxSlaveStateIsOldModel = new QCheckBox;
	chBoxSlaveStateIsInpulseCommand = new QCheckBox;
	chBoxSlaveStateIsEMZMmodel = new QCheckBox;
	comBoxSlaveStateModeOrCounter = new QComboBox;

	lEdTimeoutSlaveRespond = new QLineEdit;

	comBoxCodeType = new QComboBox;
	lEdProgramVersion = new QLineEdit;
	lEdPhysicalAddress = new QLineEdit;
	lEdSerialNumber = new QLineEdit;
	lEdProgramCode = new QLineEdit;
	lEdLogicalAddress = new QLineEdit;
}

TabSybusMasterIdentification::~TabSybusMasterIdentification()
{
}


bool TabSybusMasterIdentification::build(void) {

	// Настройка списка ведомых
	QLabel *labChooseSlave = new QLabel;
	labChooseSlave->setText("Выберите ведомого: ");

	QLabel *labChooseSlaveDescr = new QLabel;
	labChooseSlaveDescr->setText("Количество ведомых устанавливается на вкладке \"Параметры протокола\"");

	updateSlaveList();		// Обновляет КомбоБокс с доступными ведомыми
	QObject::connect(comBoxChooseSlave, SIGNAL(currentIndexChanged(int)), this, SLOT(slotChangeComBoxCurrentSlaveIndex(int)));

	int maxSlaveCountOfAllPorts = getParameterCount(SP_USARTEXTNET_DEFINITION);
	int portCount = getParameterCount(SP_USARTPRTPAR_BYTE);

	if (portCount != 0) {
		m_maxSlaveCount = maxSlaveCountOfAllPorts / portCount;		// Рассчитываем кол-во слейвов на одном порте
	}
	else {
		m_maxSlaveCount = 0;
	}
	

	// Настройка режима слейва
    auto currentSlaveOptions = m_controller->paramsRegistry().element(SP_USARTEXTNET_DEFINITION, getIndexAtAddressForSlave(ns_slaveModeDefines::SLAVE_MODE));
	CHECK_ADDRESS_RETURN_BOOL(currentSlaveOptions);
	

	int currentSlaveMode = currentSlaveOptions->value().toInt();

	chBoxSlaveStateIsActive->setText("Активный (скофигурирован)");
	if (currentSlaveMode & ns_slaveModeDefines::FLAG_SLAVE_ACTIVE) chBoxSlaveStateIsActive->setChecked(true);
	else chBoxSlaveStateIsActive->setChecked(false);
	QObject::connect(chBoxSlaveStateIsActive, SIGNAL(stateChanged(int)), this, SLOT(slotChangeChBoxSlaveStateIsActive(int)));

	chBoxSlaveStateIsOldModel->setText("Слейв из устаревшей линейки модулей на 8-ми разрядных  процессорах");
	if (currentSlaveMode & ns_slaveModeDefines::FLAG_SLAVE_OLD) chBoxSlaveStateIsOldModel->setChecked(true);
	else chBoxSlaveStateIsOldModel->setChecked(false);
	QObject::connect(chBoxSlaveStateIsOldModel, SIGNAL(stateChanged(int)), this, SLOT(slotChangeChBoxSlaveStateIsOldModel(int)));

	chBoxSlaveStateIsInpulseCommand->setText("Слейв поддерживает  только импульсные команды управления ТУ");
	if (currentSlaveMode & ns_slaveModeDefines::FLAG_SLAVE_IMPULSE) chBoxSlaveStateIsInpulseCommand->setChecked(true);
	else chBoxSlaveStateIsInpulseCommand->setChecked(false);
	QObject::connect(chBoxSlaveStateIsInpulseCommand, SIGNAL(stateChanged(int)), this, SLOT(slotChangeChBoxSlaveStateIsInpulseCommand(int)));

	chBoxSlaveStateIsEMZMmodel->setText("Слейв из категории представления счетчиков  ЭЭ EM3M");		// !!! Зависит от выбора chBoxSlaveStateIsInpulseCommand!!!
	if (currentSlaveMode & ns_slaveModeDefines::FLAG_SLAVE_EMZM_TYPE) {
		chBoxSlaveStateIsEMZMmodel->setChecked(true);
		comBoxSlaveStateModeOrCounter->addItem(STR_PHASE_MODE);
		comBoxSlaveStateModeOrCounter->addItem(STR_SUMM_MODE);
	}
	else {
		chBoxSlaveStateIsEMZMmodel->setChecked(false);
		comBoxSlaveStateModeOrCounter->addItem(STR_4BIT_COUNTER);
		comBoxSlaveStateModeOrCounter->addItem(STR_8BIT_COUNTER);
	}

	if (currentSlaveMode & ns_slaveModeDefines::FLAG_SLAVE_MODE_OR_COUNT) comBoxSlaveStateModeOrCounter->setCurrentIndex(1);
	else comBoxSlaveStateModeOrCounter->setCurrentIndex(0);
	QObject::connect(chBoxSlaveStateIsEMZMmodel, SIGNAL(stateChanged(int)), this, SLOT(slotChangeChBoxSlaveStateIsEMZMmodel(int)));

	QObject::connect(comBoxSlaveStateModeOrCounter, SIGNAL(currentIndexChanged(const QString &)), this, SLOT(slotChangeComBoxSlaveStateModeOrCounter(const QString &)));

    auto paramSlaveCount = m_controller->paramsRegistry().element(SP_USARTPRTPAR_BYTE, m_port_id + Rs485::Indexs::Byte::SlaveCount);
	CHECK_ADDRESS_RETURN_BOOL(paramSlaveCount);



	QGroupBox *frameSlaveMode = new QGroupBox;					// Рамка для настройки ведомых
	QVBoxLayout *vBoxSlaveWidget = new QVBoxLayout;					// Вертикальное размещение
	frameSlaveMode->setLayout(vBoxSlaveWidget);

	vBoxSlaveWidget->addWidget(chBoxSlaveStateIsActive);					// Добавляем выбор ведомого	
	vBoxSlaveWidget->addWidget(chBoxSlaveStateIsOldModel);					// Добавляем выбор ведомого	
	vBoxSlaveWidget->addWidget(chBoxSlaveStateIsInpulseCommand);			// Добавляем выбор ведомого	
	vBoxSlaveWidget->addWidget(chBoxSlaveStateIsEMZMmodel);					// Добавляем выбор ведомого	
	vBoxSlaveWidget->addWidget(comBoxSlaveStateModeOrCounter);

	// Таймуат ответа слейва
    auto currentTimeout = m_controller->paramsRegistry().element(SP_USARTEXTNET_DEFINITION, getIndexAtAddressForSlave(ns_slaveModeDefines::TIMEOUT));
	CHECK_ADDRESS_RETURN_BOOL(currentTimeout);

	lEdTimeoutSlaveRespond->setText(currentTimeout->value());
	if (!checkIsValidValue(lEdTimeoutSlaveRespond->text().toInt(), LIMIT_TIMEOUT_SLAVE_RESPOND_MIN, LIMIT_TIMEOUT_SLAVE_RESPOND_MAX)) {
		lEdTimeoutSlaveRespond->setStyleSheet(ERR_QLINEEDIT_STYLE_SHEET);
	}

	QObject::connect(lEdTimeoutSlaveRespond, SIGNAL(editingFinished()), this, SLOT(slotChangeTEdTimeoutSlaveRespond()));


	///// Сложная часть

	// Идентификация слейва
	for (QPair<QString, int> type: vectorTypeCodesDefine) {			// Здесь должны быть коды типа
		vectorTypeCodes.push_back(type);
		comBoxCodeType->addItem(type.first);
	}

	vectorLineEditIdentification.push_back(lEdProgramVersion);		// Заносим все текстовые поля в вектор
	vectorLineEditIdentification.push_back(lEdPhysicalAddress);
	vectorLineEditIdentification.push_back(lEdSerialNumber);
	vectorLineEditIdentification.push_back(lEdProgramCode);
	vectorLineEditIdentification.push_back(lEdLogicalAddress);

	for(QLineEdit* lineEdit: vectorLineEditIdentification) {	// Заполянем текст 
		lineEdit->setPlaceholderText(STR_SET_THE_VALUE);
	}	

	// Вычитываем длину блока XID, Идентификация ведомого. потом это будет раскладываться на все вышеописанные чекбоксы и тексты
    auto currentIdentificationLen = m_controller->paramsRegistry().element(SP_USARTEXTNET_DEFINITION, getIndexAtAddressForSlave(ns_slaveModeDefines::XID_BLOCK_LEN));
	CHECK_ADDRESS_RETURN_BOOL(currentIdentificationLen);
	
	int identLen = currentIdentificationLen->value().toInt();	// Сохраняем длину 

	if (identLen != 0) {	// Если длина не ноль, то читаем коды и выставляем чекбоксы и пишем в поля текст		
		updateCodes();
	}

	// Присоединяем все сигналы
	QObject::connect(comBoxCodeType, SIGNAL(currentIndexChanged(const QString &)), this, SLOT(slotChangeComBoxCodeType(const QString &)));  
	QObject::connect(lEdProgramVersion, SIGNAL(editingFinished()), this, SLOT(slotChangeLEdVersion()));
	QObject::connect(lEdPhysicalAddress, SIGNAL(editingFinished()), this, SLOT(slotChangeLEdPhysicalAddress()));
	QObject::connect(lEdSerialNumber, SIGNAL(editingFinished()), this, SLOT(slotChangeLEdSerialNumber()));
	QObject::connect(lEdProgramCode, SIGNAL(editingFinished()), this, SLOT(slotChangeLEdProgramCode()));
	QObject::connect(lEdLogicalAddress, SIGNAL(editingFinished()), this, SLOT(slotChangeLEdLogicalAddress()));

	   

	QGroupBox *frameSlaveIdentification = new QGroupBox;		// Рамка для настройки ведомых
	QFormLayout *fLaySlaveIdentification = new QFormLayout;
	frameSlaveIdentification->setLayout(fLaySlaveIdentification);

	fLaySlaveIdentification->addRow(new QLabel("Код типа"), comBoxCodeType);
	fLaySlaveIdentification->addRow(new QLabel("Версия ПО"), lEdProgramVersion);
	fLaySlaveIdentification->addRow(new QLabel("Физический адрес"), lEdPhysicalAddress);
	fLaySlaveIdentification->addRow(new QLabel("Серийный номер"), lEdSerialNumber);
	fLaySlaveIdentification->addRow(new QLabel("Код ПО"), lEdProgramCode);
	fLaySlaveIdentification->addRow(new QLabel("Логический адрес"), lEdLogicalAddress);



	// Настройка таблицы отображения информации о ведомых
	int row = 0;
	QGridLayout *slaveTable = new QGridLayout;
	slaveTable->addWidget(labChooseSlave, row, TABLE_COLOM_PARAMETER_NAME);
	slaveTable->addWidget(comBoxChooseSlave, row, TABLE_COLOM_INPUT_OBJECT);
	slaveTable->addWidget(labChooseSlaveDescr, row, TABLE_COLOM_DESCRIPTION);
	row++;

	QLabel *labSlaveMode = new QLabel;
	labSlaveMode->setText("Опции ведомого");
	slaveTable->addWidget(labSlaveMode, row, 0);
	slaveTable->addWidget(frameSlaveMode, row, 1);
	row++;

	QString strTimeoutSlaveRespondDescription = makeStrDefultAndLimitValue(DEFAULT_TIMEOUT_SLAVE_RESPOND, LIMIT_TIMEOUT_SLAVE_RESPOND_MIN, LIMIT_TIMEOUT_SLAVE_RESPOND_MAX);
	slaveTable->addWidget(new QLabel("Таймаут ответа ведомого, мс"), row, TABLE_COLOM_PARAMETER_NAME);
	slaveTable->addWidget(lEdTimeoutSlaveRespond, row, TABLE_COLOM_INPUT_OBJECT);
	slaveTable->addWidget(new QLabel(strTimeoutSlaveRespondDescription), row, TABLE_COLOM_DESCRIPTION);
	row++;

	//QString strTimeoutSlaveRespondDescription = makeStrDefultAndLimitValue(DEFAULT_TIMEOUT_SLAVE_RESPOND, LIMIT_TIMEOUT_SLAVE_RESPOND_MIN, LIMIT_TIMEOUT_SLAVE_RESPOND_MAX);
	slaveTable->addWidget(new QLabel("Идентификация ведомого"), row, TABLE_COLOM_PARAMETER_NAME);
	slaveTable->addWidget(frameSlaveIdentification, row, TABLE_COLOM_INPUT_OBJECT);
	//slaveTable->addWidget(new QLabel(strTimeoutSlaveRespondDescription), row, TABLE_COLOM_DESCRIPTION);
	row++;

	m_connectionDiscret = new ConnectionDiscretWidget(m_controller, m_portNumber, currentSlaveNumber, QString("Sybus Слэйв(%1:%2)").arg(m_portNumber).arg(currentSlaveNumber), widIdentification);
	slaveTable->addWidget(new QLabel("Дискрет связи"), row, TABLE_COLOM_PARAMETER_NAME);
	slaveTable->addWidget(m_connectionDiscret, row, TABLE_COLOM_INPUT_OBJECT);
	row++;


	slaveTable->setRowStretch(row, 100);			// Устанавливает растяжение пустых строк и столбцов в конце
	slaveTable->setColumnStretch(5, 100);


	// Добавление таблицы в виджет
	widIdentification->setLayout(slaveTable);

	return true;
}

QWidget* TabSybusMasterIdentification::formIdentification(void) {
	return widIdentification;
}

int32_t TabSybusMasterIdentification::error() {
	return 666;
}

QString TabSybusMasterIdentification::errorMessage() {
	return strErrorMessage;
}


int TabSybusMasterIdentification::getParameterCount(int address) {

	int maxCount = 0;

	// Начинаем пробавать вычитывать значения по 0 порту, полагая, что кол-во слейвов для всех портов одинаковое
	int index = 0;
    auto checkSlave = m_controller->paramsRegistry().element(address, index);

	while (checkSlave != nullptr) {		// Пока считывается нормально
		maxCount++;						// Увеличиваем число доступных элементов
		index += 256;					// Для одного элемента максимум отводится 256 байт (0..255)
        checkSlave = m_controller->paramsRegistry().element(address, index);	// Считываем с новой позиции
	}

	return maxCount;

}

bool TabSybusMasterIdentification::checkIsValidValue(int param, int min, int max) {
	if (param >= min && param <= max) return true;
	return false;
}

QString TabSybusMasterIdentification::makeStrOfLimits(int min, int max) {
	QString strLimits = STR_RANGE_OF_VALUES_IS + QString::number(min) + "..." + QString::number(max);
	return strLimits;
}



QString TabSybusMasterIdentification::makeStrDefultAndLimitValue(int def, int min, int max) { // Выдает строку с значением по умолчанию и диапазоном мин/макс
	QString outStr = STR_DEFAULT_PARAMETER_IS + QString::number(def) + "; \t" + makeStrOfLimits(min, max);
	return outStr;
}

void TabSybusMasterIdentification::blockIdentificatorSignals(bool state) {		// Блокируем сигналы 
	
	chBoxSlaveStateIsActive->blockSignals(state);
	chBoxSlaveStateIsEMZMmodel->blockSignals(state);
	chBoxSlaveStateIsInpulseCommand->blockSignals(state);
	chBoxSlaveStateIsOldModel->blockSignals(state);
	comBoxSlaveStateModeOrCounter->blockSignals(state);

	lEdTimeoutSlaveRespond->blockSignals(state);

	for(QLineEdit* lineEdit: vectorLineEditIdentification) {
		lineEdit->blockSignals(state);
	}

	comBoxCodeType->blockSignals(state);
}


void TabSybusMasterIdentification::updateParametersAfterSlaveChange() {

    auto paramSlaveMode = m_controller->paramsRegistry().element(SP_USARTEXTNET_DEFINITION, getIndexAtAddressForSlave(ns_slaveModeDefines::SLAVE_MODE));
	CHECK_ADDRESS(paramSlaveMode);

	blockIdentificatorSignals(true);

	int intParamSlaveMode = paramSlaveMode->value().toInt();			// Преобразуем вычитанную стору в число, считаем, что записано правильно (число)	

	if (intParamSlaveMode & ns_slaveModeDefines::FLAG_SLAVE_ACTIVE) chBoxSlaveStateIsActive->setChecked(true);
	else chBoxSlaveStateIsActive->setChecked(false);
	if (intParamSlaveMode & ns_slaveModeDefines::FLAG_SLAVE_OLD) chBoxSlaveStateIsOldModel->setChecked(true);
	else chBoxSlaveStateIsOldModel->setChecked(false);
	if (intParamSlaveMode & ns_slaveModeDefines::FLAG_SLAVE_IMPULSE) chBoxSlaveStateIsInpulseCommand->setChecked(true);
	else chBoxSlaveStateIsInpulseCommand->setChecked(false);

	comBoxSlaveStateModeOrCounter->clear();
	if (intParamSlaveMode & ns_slaveModeDefines::FLAG_SLAVE_EMZM_TYPE) {
		chBoxSlaveStateIsEMZMmodel->setChecked(true);		
		comBoxSlaveStateModeOrCounter->addItem(STR_SUMM_MODE);
		comBoxSlaveStateModeOrCounter->addItem(STR_PHASE_MODE);
	}
	else {
		chBoxSlaveStateIsEMZMmodel->setChecked(false);
		comBoxSlaveStateModeOrCounter->addItem(STR_8BIT_COUNTER);
		comBoxSlaveStateModeOrCounter->addItem(STR_4BIT_COUNTER);
	}	

	if (intParamSlaveMode & ns_slaveModeDefines::FLAG_SLAVE_MODE_OR_COUNT) comBoxSlaveStateModeOrCounter->setCurrentIndex(0);
	else comBoxSlaveStateModeOrCounter->setCurrentIndex(1);

    auto paramTimeout = m_controller->paramsRegistry().element(SP_USARTEXTNET_DEFINITION, getIndexAtAddressForSlave(ns_slaveModeDefines::TIMEOUT));
	CHECK_ADDRESS(paramTimeout);
	lEdTimeoutSlaveRespond->setText(paramTimeout->value());
	
	blockIdentificatorSignals(false);

	// Проверяем сложный участок. Записи идентификационной информации
    auto paramIdentificationLen = m_controller->paramsRegistry().element(SP_USARTEXTNET_DEFINITION, getIndexAtAddressForSlave(ns_slaveModeDefines::XID_BLOCK_LEN));
	CHECK_ADDRESS(paramIdentificationLen);
	int curLen = paramIdentificationLen->value().toInt();		// Получаем длину идентификационного блока

	if (curLen != 0) {		// Если длина не ноль
		updateCodes();		// Считываем коды и обновляем форму
	}
	else { // Если длина нулевая
		cleanIdentificationForms();		// Очистить все поля формы
	}

	ConnectionDiscretWidget *newConnectionDiscret = new ConnectionDiscretWidget(m_controller, m_portNumber, currentSlaveNumber,
		QString("Sybus Слэйв(%1:%2)").arg(m_portNumber).arg(currentSlaveNumber), widIdentification);
	widIdentification->layout()->replaceWidget(m_connectionDiscret, newConnectionDiscret);
	m_connectionDiscret->deleteLater();
	m_connectionDiscret = newConnectionDiscret;
}

void TabSybusMasterIdentification::updateCodes() {
	
    auto paramIdentificationLen = m_controller->paramsRegistry().element(SP_USARTEXTNET_DEFINITION, getIndexAtAddressForSlave(ns_slaveModeDefines::XID_BLOCK_LEN));
	CHECK_ADDRESS(paramIdentificationLen);
	int curLen = paramIdentificationLen->value().toInt();		// Получаем длину идентификационного блока

	cleanIdentificationForms();		// Обнуляет все поля в начальное состояние

	blockIdentificatorSignals(true);

	for (int currInd = 0; currInd < curLen; currInd++) {	// Проверка по всем байтам

		// Читаем параметр, увеличиваем currInd++. 
        auto currentCode = m_controller->paramsRegistry().element(SP_USARTEXTNET_DEFINITION, getIndexAtAddressForSlave(ns_slaveModeDefines::XID_BLOCK_START + currInd++));
		CHECK_ADDRESS(currentCode);
		int identificator = currentCode->value().toInt();		//Получаем код

		// Читаем значение этого кода (для случаем, где код не один байт, будет читаться дальше)
		// Не увеличиваем currInd, т.к. если в коде один байт, при выходе из круга цикла, currInd увеличится
        auto currentValue = m_controller->paramsRegistry().element(SP_USARTEXTNET_DEFINITION, getIndexAtAddressForSlave(ns_slaveModeDefines::XID_BLOCK_START + currInd));
		CHECK_ADDRESS(currentValue);

		switch (identificator) {
		case CODE_TYPE_CODE: {

			for(QPair<QString, int> codePair: vectorTypeCodes) {			// Ищем в векторе пару
				if (codePair.second == currentValue->value().toInt()) {				// Если код совпадает
					comBoxCodeType->setCurrentText(codePair.first);					// Пишем значение
					break;
				}
			}
			break;
		}
		case CODE_PROGRAMM_VERSION: {
			lEdProgramVersion->setText(currentValue->value());		// Ставим текст, и здесь же должно включаться доступность чекбокса
			break;
		}

		case CODE_PHYSICAL_ADDRESS: {
			lEdPhysicalAddress->setText(currentValue->value());
			break;
		}

		case CODE_SERIAL_NUMBER: {
			uint32_t bigNumber = currentValue->value().toInt();		// Сразу кладем младший байт (он идет первый)

			for (int i = 1; i < 4; i++) {		// До трех, потому что первое значение уже считано!
				currInd++;
                currentValue = m_controller->paramsRegistry().element(SP_USARTEXTNET_DEFINITION, getIndexAtAddressForSlave(ns_slaveModeDefines::XID_BLOCK_START + currInd));
				CHECK_ADDRESS(currentValue);
				uint8_t part = currentValue->value().toInt();	// Получаем следующий байт (слева направо получаем)
				bigNumber |= (part << 8 * i);						// Заполняем, смещая значение
			}

			lEdSerialNumber->setText(QString::number(bigNumber));
			break;
		}
		case CODE_PROGRAMM_CODE: {
			uint8_t firstByte = currentValue->value().toInt();
			uint16_t bigNumber = firstByte;

			currInd++;
            currentValue = m_controller->paramsRegistry().element(SP_USARTEXTNET_DEFINITION, getIndexAtAddressForSlave(ns_slaveModeDefines::XID_BLOCK_START + currInd));
			CHECK_ADDRESS(currentValue);

			bigNumber |= (currentValue->value().toInt() << 8);
			//strSerialNumber.insert(0, currentValue->value());	// Складываем в строку					

			lEdProgramCode->setText(QString::number(bigNumber));
			break;
		}
		case CODE_LOGICAL_ADDRESS: {
			lEdLogicalAddress->setText(currentValue->value());
			break;
		}
		default: {

			break;
		}
		}
	}

	// Снова включаем сигналы
	blockIdentificatorSignals(false);

}

int TabSybusMasterIdentification::getIndexAtAddressForSlave(int parameterIndex) {
	if (currentSlaveNumber < 0) {
		MsgBox::warning("Текущий индекс слейва отрицательный, нельзя сформировать адрес!");
	}
	int slaveIndex = m_portNumber * m_maxSlaveCount * 256;	// В порту maxSlaveCount слейвов, на каждый слейв 256 байт

	return (slaveIndex + currentSlaveNumber * 256 + parameterIndex);		// На каждом порту несколько слейвов может быть
}


void TabSybusMasterIdentification::updateSlaveList() {

    auto slaveCount = m_controller->paramsRegistry().element(SP_USARTPRTPAR_BYTE, m_port_id + Rs485::Indexs::Byte::SlaveCount);
	CHECK_ADDRESS(slaveCount);

	currentSlaveNumber = 0;			// Переходим на первого слейва

	comBoxChooseSlave->clear();															// Очищаем предыдущий список

	QString strSlaveName;
	if (slaveCount->value().toInt() != 0) {			// Если есть слейвы		
		for (int i = 0; i < slaveCount->value().toInt(); i++) {								// Заполняем список из номеров ведомых
			strSlaveName = STR_SLAVE + QString::number(i + 1);								// Слейвы начинаются с 1! Для удобства пользователя
			comBoxChooseSlave->addItem(strSlaveName);
		}		
	}
	else {	// Теперь это будет недоступно, т.к. вся вкладка будет неактивна
		//makeAllDisable(true);
		//cleanSlaveInfo();
	}

}




void TabSybusMasterIdentification::cleanSlaveIdentificationInModel() {

    auto currentIdentificationLen = m_controller->paramsRegistry().element(SP_USARTEXTNET_DEFINITION, getIndexAtAddressForSlave(ns_slaveModeDefines::XID_BLOCK_LEN));
	CHECK_ADDRESS(currentIdentificationLen);
	currentIdentificationLen->updateValue(QString::number(0));

	for (int i = 0; i < LIMIT_XID_LEN_MAX; i++) {
        auto currentCode = m_controller->paramsRegistry().element(SP_USARTEXTNET_DEFINITION, getIndexAtAddressForSlave(ns_slaveModeDefines::XID_BLOCK_START + i));
		currentCode->updateValue(QString::number(0));
	}

}

void TabSybusMasterIdentification::cleanIdentificationForms() {
	   
	// Обнуляем все редакторы текста
	for(QLineEdit* lineEdit: vectorLineEditIdentification) {
		lineEdit->blockSignals(true);
		lineEdit->clear();
		lineEdit->blockSignals(false);
	}

	// Обнуляем комбоБокс!!!
	comBoxCodeType->blockSignals(true);
	comBoxCodeType->setCurrentText(STR_TYPE_CODE_NONE);
	comBoxCodeType->blockSignals(false);

}

QVector<QString>* TabSybusMasterIdentification::getErrorVectorOfStr() {

	return &vectorErrorStringsIdentification;
}
	
//QVector<QString>* slaveForSybusMasterIdentification::checkSlaveSettingsIdentification() {						// Проверяет, все ли параметры заполнены
//	
//	int oldSlaveIndex = currentSlaveNumber;				// Сохраняем текущий индекс
//	vectorErrorStringsIdentification.clear();			// Очищаем вектор ошибок
//	QString strSlaveNumber;
//		 
//	for (int slaveNumber = 0; slaveNumber < comBoxChooseSlave->count(); slaveNumber++) {		// Проходим по всем слейвам 
//		currentSlaveNumber = slaveNumber;														// Обновляем номер текущего слейва
//		strSlaveNumber = STR_SLAVE + QString::number(slaveNumber + 1) + ". ";					// Для пользователя все слейвы начинаются с 1!	
//
//		updateParametersAfterSlaveChange();		// Обновляем инфо, как будто это он
//
//		// Начинаем проверки необходимых условий!
//		if (!isOneIdentificationParameterChoosen()) {
//			QString strForWrite = strSlaveNumber + ERR_STR_WRONG_COUNT_OF_IDENTIF_PARAM;
//			vectorErrorStringsIdentification.push_back(strForWrite);
//		}
//
//		if (!chBoxSlaveStateIsActive->isChecked()) {
//			QString strForWrite = strSlaveNumber + ERR_STR_SLAVE_NOT_ACTIVE;
//			vectorErrorStringsIdentification.push_back(strForWrite);
//		}
//	}
//
//
//	currentSlaveNumber = oldSlaveIndex;		// Возвращаем номер
//	updateParametersAfterSlaveChange();		// Обновляем инфо для старого индекса
//
//	return &vectorErrorStringsIdentification;
//}


bool TabSybusMasterIdentification::isAllParametersValid() {
	bool noError = true;

	int oldSlaveIndex = currentSlaveNumber;				// Сохраняем текущий индекс
	vectorErrorStringsIdentification.clear();			// Очищаем вектор ошибок
	QString strSlaveNumber;

	for (int slaveNumber = 0; slaveNumber < comBoxChooseSlave->count(); slaveNumber++) {		// Проходим по всем слейвам 
		currentSlaveNumber = slaveNumber;														// Обновляем номер текущего слейва
		strSlaveNumber = STR_SLAVE + QString::number(slaveNumber + 1) + ". ";					// Для пользователя все слейвы начинаются с 1!	

		updateParametersAfterSlaveChange();		// Обновляем инфо, как будто это он

		// Начинаем проверки необходимых условий!
		if (!isOneIdentificationParameterChoosen()) {
			QString strForWrite = strSlaveNumber + ERR_STR_WRONG_COUNT_OF_IDENTIF_PARAM;
			vectorErrorStringsIdentification.push_back(strForWrite);
			noError = false;
		}

		if (!chBoxSlaveStateIsActive->isChecked()) {
			QString strForWrite = strSlaveNumber + ERR_STR_SLAVE_NOT_ACTIVE;
			vectorErrorStringsIdentification.push_back(strForWrite);
			noError = false;
		}
	}

	if (!isTimeoutValid()) {
		vectorErrorStringsIdentification.push_back(ERR_STR_TIMEOUT_SUMM_EXCEED);
		noError = false;
	}

	currentSlaveNumber = oldSlaveIndex;		// Возвращаем номер
	updateParametersAfterSlaveChange();		// Обновляем инфо для старого индекса

	return noError;
}

bool TabSybusMasterIdentification::isOneIdentificationParameterChoosen(){						// Проверет, выбран ли хотя бы один из параметров идентификации

	int countOfChoosenBox = 0;			// Количество выбранных чекбоксов

	for(QLineEdit* lineEdit: vectorLineEditIdentification) {		// проходим по всем боксам
		if (!lineEdit->text().isEmpty()) countOfChoosenBox++;					// Если стоит галочка, значит значение подходит
	}

	if (countOfChoosenBox < LIMIT_XID_COUNT_MIN) {
		//MsgBox::error(ERR_STR_WRONG_COUNT_OF_IDENTIF_PARAM);
		return false;
	} 
	
	return true;
}

bool TabSybusMasterIdentification::isTimeoutValid() {		// Не отлажено!!!
	bool isValid = true;

    auto paramSlaveCount = m_controller->paramsRegistry().element(SP_USARTPRTPAR_BYTE, m_port_id + Rs485::Indexs::Byte::SlaveCount);	// Узнаем кол-во ведомых на этом порте
	CHECK_ADDRESS_RETURN_BOOL(paramSlaveCount);

	int slaveCount = paramSlaveCount->value().toInt();	// Получаем число ведомых
	if (slaveCount == 0) return isValid;		// Если вдруг ноль, то все верно (хотя этого не должно быть)

	int timeoutSum = 0;							// Будем суммировать таймауты
	for (int slave = 0; slave < slaveCount; slave++) {

		int slaveIndex = m_portNumber * m_maxSlaveCount * 256;		// Смещения в зависимости от выбранного порта
		int addr = slaveIndex + slave * 256 + ns_slaveModeDefines::TIMEOUT;	// Расчет адреса текущего слейва на данном порте

        auto paramSlaveTimeout = m_controller->paramsRegistry().element(SP_USARTEXTNET_DEFINITION, addr);	// Узнаем кол-во ведомых
		CHECK_ADDRESS_RETURN_BOOL(paramSlaveCount);
		timeoutSum += paramSlaveTimeout->value().toInt();
	}

    auto paramPollingInterval = m_controller->paramsRegistry().element(SP_USARTPRTPAR_WORD, m_port_id + Rs485::Indexs::Word::Interval);	// Узнаем кол-во ведомых
	CHECK_ADDRESS_RETURN_BOOL(paramPollingInterval);
	int pollingInterval = paramPollingInterval->value().toInt();

	if (timeoutSum > pollingInterval) {		// Проверяем, если сумма таймаутов больше интервалов опроса, значит ошибка!
		isValid = false;
	}

	return isValid;
}



bool TabSybusMasterIdentification::updateIdenAfterValueEntering() {

	int updateLen = 0;		// Будем считать длину и в конце запишем
	int index = 0;			// Смещение от XID_BLOCK_START

	cleanSlaveIdentificationInModel();			// Обнуляем все 

	if (comBoxCodeType->currentIndex() != 0) {
		updateLen += 2;					// На это параметр уйдет 2 байта
        auto currentIdentificationCode = m_controller->paramsRegistry().element(SP_USARTEXTNET_DEFINITION, getIndexAtAddressForSlave(ns_slaveModeDefines::XID_BLOCK_START + index++));
		CHECK_ADDRESS_RETURN_BOOL(currentIdentificationCode);
		currentIdentificationCode->updateValue(QString::number(CODE_TYPE_CODE));	// Пишем код
        auto currentIdentificationValue = m_controller->paramsRegistry().element(SP_USARTEXTNET_DEFINITION, getIndexAtAddressForSlave(ns_slaveModeDefines::XID_BLOCK_START + index++));
		CHECK_ADDRESS_RETURN_BOOL(currentIdentificationValue);

		for(QPair<QString, int> codePair: vectorTypeCodes) {
			if (codePair.first == comBoxCodeType->currentText()) {
				currentIdentificationValue->updateValue(QString::number(codePair.second));					// Пишем значение
				break;
			}
		}
		
	}

	if (!lEdProgramVersion->text().isEmpty()) {
		updateLen += 2;					// На это параметр уйдет 2 байта
        auto currentIdentificationCode = m_controller->paramsRegistry().element(SP_USARTEXTNET_DEFINITION, getIndexAtAddressForSlave(ns_slaveModeDefines::XID_BLOCK_START + index++));
		CHECK_ADDRESS_RETURN_BOOL(currentIdentificationCode);
		currentIdentificationCode->updateValue(QString::number(CODE_PROGRAMM_VERSION));	// Пишем код
        auto currentIdentificationValue = m_controller->paramsRegistry().element(SP_USARTEXTNET_DEFINITION, getIndexAtAddressForSlave(ns_slaveModeDefines::XID_BLOCK_START + index++));
		CHECK_ADDRESS_RETURN_BOOL(currentIdentificationValue);
		currentIdentificationValue->updateValue(lEdProgramVersion->text());					// Пишем значение
	}

	if (!lEdPhysicalAddress->text().isEmpty()) {
		updateLen += 2;					// На это параметр уйдет 2 байта, один на код, другой на значение
        auto currentIdentificationCode = m_controller->paramsRegistry().element(SP_USARTEXTNET_DEFINITION, getIndexAtAddressForSlave(ns_slaveModeDefines::XID_BLOCK_START + index++));
		CHECK_ADDRESS_RETURN_BOOL(currentIdentificationCode);
		currentIdentificationCode->updateValue(QString::number(CODE_PHYSICAL_ADDRESS));	// Пишем код
        auto currentIdentificationValue = m_controller->paramsRegistry().element(SP_USARTEXTNET_DEFINITION, getIndexAtAddressForSlave(ns_slaveModeDefines::XID_BLOCK_START + index++));
		CHECK_ADDRESS_RETURN_BOOL(currentIdentificationValue);
		currentIdentificationValue->updateValue(lEdPhysicalAddress->text());					// Пишем значение
	}

	if (!lEdSerialNumber->text().isEmpty()) {
		updateLen += 5;					// На это параметр уйдет 5 байт, один на код, и 4 на данные
        auto currentIdentificationCode = m_controller->paramsRegistry().element(SP_USARTEXTNET_DEFINITION, getIndexAtAddressForSlave(ns_slaveModeDefines::XID_BLOCK_START + index++));
		CHECK_ADDRESS_RETURN_BOOL(currentIdentificationCode);
		currentIdentificationCode->updateValue(QString::number(CODE_SERIAL_NUMBER));	// Пишем код

		int16_t oneByte = 0;
		int32_t fullSerialNumber = lEdSerialNumber->text().toLongLong();		// Преобразуем в 4 байта

		for (int i = 0; i < 4 ; i++) {
			oneByte = fullSerialNumber & 0xff;
			fullSerialNumber = fullSerialNumber >> 8;
            auto currentIdentificationValue = m_controller->paramsRegistry().element(SP_USARTEXTNET_DEFINITION, getIndexAtAddressForSlave(ns_slaveModeDefines::XID_BLOCK_START + index++));
			CHECK_ADDRESS_RETURN_BOOL(currentIdentificationValue);
			currentIdentificationValue->updateValue(QString::number(oneByte));					// Пишем значение
		}

	}

	if (!lEdProgramCode->text().isEmpty()) {
		updateLen += 3;					// На это параметр уйдет 3 байт, один на код, и 2 на данные 
        auto currentIdentificationCode = m_controller->paramsRegistry().element(SP_USARTEXTNET_DEFINITION, getIndexAtAddressForSlave(ns_slaveModeDefines::XID_BLOCK_START + index++));
		CHECK_ADDRESS_RETURN_BOOL(currentIdentificationCode);
		currentIdentificationCode->updateValue(QString::number(CODE_PROGRAMM_CODE));	// Пишем код

		int8_t oneByte = 0;
		int16_t fullSerialNumber = lEdProgramCode->text().toLongLong();		// Преобразуем в 4 байта

		for (int i = 0; i < 2; i++) {
			oneByte = fullSerialNumber & 0xff;
			fullSerialNumber = fullSerialNumber >> 8;
            auto currentIdentificationValue = m_controller->paramsRegistry().element(SP_USARTEXTNET_DEFINITION, getIndexAtAddressForSlave(ns_slaveModeDefines::XID_BLOCK_START + index++));
			CHECK_ADDRESS_RETURN_BOOL(currentIdentificationValue);
			currentIdentificationValue->updateValue(QString::number(oneByte));					// Пишем значение
		}

	}

	if (!lEdLogicalAddress->text().isEmpty()) {
		updateLen += 2;					// На это параметр уйдет 2 байта, один на код, другой на значение
        auto currentIdentificationCode = m_controller->paramsRegistry().element(SP_USARTEXTNET_DEFINITION, getIndexAtAddressForSlave(ns_slaveModeDefines::XID_BLOCK_START + index++));
		CHECK_ADDRESS_RETURN_BOOL(currentIdentificationCode);
		currentIdentificationCode->updateValue(QString::number(CODE_LOGICAL_ADDRESS));	// Пишем код
        auto currentIdentificationValue = m_controller->paramsRegistry().element(SP_USARTEXTNET_DEFINITION, getIndexAtAddressForSlave(ns_slaveModeDefines::XID_BLOCK_START + index++));
		CHECK_ADDRESS_RETURN_BOOL(currentIdentificationValue);
		currentIdentificationValue->updateValue(lEdLogicalAddress->text());					// Пишем значение
	}
	   
	if (updateLen > LIMIT_XID_LEN_MAX) {
		MsgBox::warning("Превышено допустимое кол-во байт!");
		return false;
	}
    auto currentIdentificationLen = m_controller->paramsRegistry().element(SP_USARTEXTNET_DEFINITION, getIndexAtAddressForSlave(ns_slaveModeDefines::XID_BLOCK_LEN));
	CHECK_ADDRESS_RETURN_BOOL(currentIdentificationLen);
	currentIdentificationLen->updateValue(QString::number(updateLen));



	return true;
}






void  TabSybusMasterIdentification::slotChangeComBoxCurrentSlaveIndex(int newSlaveIndex) {

	currentSlaveNumber = newSlaveIndex;
	if (newSlaveIndex < 0) currentSlaveNumber = 0;
	updateParametersAfterSlaveChange();
}

void TabSybusMasterIdentification::slotChangeChBoxSlaveStateIsActive(int state) {		// Слоты настройки режима ведомого

    auto currentVal = m_controller->paramsRegistry().element(SP_USARTEXTNET_DEFINITION, getIndexAtAddressForSlave(ns_slaveModeDefines::SLAVE_MODE));
	CHECK_ADDRESS(currentVal);

	int oldVal = currentVal->value().toInt();

	if (state == Qt::Checked) {
		int newVal = oldVal | ns_slaveModeDefines::FLAG_SLAVE_ACTIVE;			// Включаем флаг
		currentVal->updateValue(QString::number(newVal));
	}
	else {
		int newVal = oldVal & (~ns_slaveModeDefines::FLAG_SLAVE_ACTIVE);		// Выключаем флаг
		currentVal->updateValue(QString::number(newVal));
	}

}

void TabSybusMasterIdentification::slotChangeChBoxSlaveStateIsOldModel(int state) {
    auto currentVal = m_controller->paramsRegistry().element(SP_USARTEXTNET_DEFINITION, getIndexAtAddressForSlave(ns_slaveModeDefines::SLAVE_MODE));
	CHECK_ADDRESS(currentVal);

	int oldVal = currentVal->value().toInt();

	if (state == Qt::Checked) {
		int newVal = oldVal | ns_slaveModeDefines::FLAG_SLAVE_OLD;	// Включаем флаг
		currentVal->updateValue(QString::number(newVal));
	}
	else {
		int newVal = oldVal & (~ns_slaveModeDefines::FLAG_SLAVE_OLD);	// Выключаем флаг
		currentVal->updateValue(QString::number(newVal));
	}
}

void TabSybusMasterIdentification::slotChangeChBoxSlaveStateIsInpulseCommand(int state) {
    auto currentVal = m_controller->paramsRegistry().element(SP_USARTEXTNET_DEFINITION, getIndexAtAddressForSlave(ns_slaveModeDefines::SLAVE_MODE));
	CHECK_ADDRESS(currentVal);

	int oldVal = currentVal->value().toInt();

	if (state == Qt::Checked) {
		int newVal = oldVal | ns_slaveModeDefines::FLAG_SLAVE_IMPULSE;	// Включаем флаг
		currentVal->updateValue(QString::number(newVal));
	}
	else {
		int newVal = oldVal & (~ns_slaveModeDefines::FLAG_SLAVE_IMPULSE);	// Выключаем флаг
		currentVal->updateValue(QString::number(newVal));
	}
}

void TabSybusMasterIdentification::slotChangeChBoxSlaveStateIsEMZMmodel(int state) {
    auto currentVal = m_controller->paramsRegistry().element(SP_USARTEXTNET_DEFINITION, getIndexAtAddressForSlave(ns_slaveModeDefines::SLAVE_MODE));
	CHECK_ADDRESS(currentVal);

	int oldVal = currentVal->value().toInt();
	comBoxSlaveStateModeOrCounter->blockSignals(true);
	comBoxSlaveStateModeOrCounter->clear();				// Очищаем все поля

	if (state == Qt::Checked) {		// Если Слейв из категории представления счетчиков  ЭЭ EM3M

		comBoxSlaveStateModeOrCounter->addItem(STR_SUMM_MODE);
		comBoxSlaveStateModeOrCounter->addItem(STR_PHASE_MODE);

		if (oldVal & ns_slaveModeDefines::FLAG_SLAVE_MODE_OR_COUNT) {			// если включен флаг
			comBoxSlaveStateModeOrCounter->setCurrentText(STR_SUMM_MODE);
		}
		else {
			comBoxSlaveStateModeOrCounter->setCurrentText(STR_PHASE_MODE);
		}

		int newVal = oldVal | ns_slaveModeDefines::FLAG_SLAVE_EMZM_TYPE;	// Включаем флаг
		currentVal->updateValue(QString::number(newVal));
	}
	else {							// Если нет
		comBoxSlaveStateModeOrCounter->addItem(STR_8BIT_COUNTER);
		comBoxSlaveStateModeOrCounter->addItem(STR_4BIT_COUNTER);

		if (oldVal & ns_slaveModeDefines::FLAG_SLAVE_MODE_OR_COUNT) {			// если включен флаг
			comBoxSlaveStateModeOrCounter->setCurrentText(STR_8BIT_COUNTER);
		}
		else {
			comBoxSlaveStateModeOrCounter->setCurrentText(STR_4BIT_COUNTER);
		}

		int newVal = oldVal & (~ns_slaveModeDefines::FLAG_SLAVE_EMZM_TYPE);	// Выключаем флаг
		currentVal->updateValue(QString::number(newVal));
	}


	comBoxSlaveStateModeOrCounter->blockSignals(false);
}


void TabSybusMasterIdentification::slotChangeComBoxSlaveStateModeOrCounter(const QString & state) {
	
	if (state.isEmpty())
		return;
	
    auto currentVal = m_controller->paramsRegistry().element(SP_USARTEXTNET_DEFINITION, getIndexAtAddressForSlave(ns_slaveModeDefines::SLAVE_MODE));
	CHECK_ADDRESS(currentVal);

	int oldVal = currentVal->value().toInt();
	int newVal;

	if (oldVal & ns_slaveModeDefines::FLAG_SLAVE_EMZM_TYPE) {		// Слейв из категории представления счетчиков  ЭЭ EM3M
		if (state == STR_SUMM_MODE) {
			newVal = oldVal | ns_slaveModeDefines::FLAG_SLAVE_MODE_OR_COUNT;	// Включаем флаг
			currentVal->updateValue(QString::number(newVal));
		}
		else {
			newVal = oldVal & (~ns_slaveModeDefines::FLAG_SLAVE_MODE_OR_COUNT);	// Выключаем флаг
			currentVal->updateValue(QString::number(newVal));
		}
	}
	else {
		if (state == STR_8BIT_COUNTER) {
			newVal = oldVal | ns_slaveModeDefines::FLAG_SLAVE_MODE_OR_COUNT;	// Включаем флаг
			currentVal->updateValue(QString::number(newVal));
		}
		else {
			newVal = oldVal & (~ns_slaveModeDefines::FLAG_SLAVE_MODE_OR_COUNT);	// Выключаем флаг
			currentVal->updateValue(QString::number(newVal));
		}
	}

}


void TabSybusMasterIdentification::slotChangeTEdTimeoutSlaveRespond() {

	bool isInt;
	int newParam = lEdTimeoutSlaveRespond->text().toInt(&isInt);						// Пробуем преобразовать в число								// Проверяем, было ли введено число

	if (!isInt || !checkIsValidValue(newParam, LIMIT_TIMEOUT_SLAVE_RESPOND_MIN, LIMIT_TIMEOUT_SLAVE_RESPOND_MAX)) {	// Если введено не число или вне диапазона
		if (lEdTimeoutSlaveRespond->text() != ERR_STR_TIMEOUT_SUMM_EXCEED) {
			lEdTimeoutSlaveRespond->setText(ERRORTEXT_NOT_IN_RANGE);
			lEdTimeoutSlaveRespond->setStyleSheet(ERR_QLINEEDIT_STYLE_SHEET);
		}		
		return;
	}	


    auto currentVal = m_controller->paramsRegistry().element(SP_USARTEXTNET_DEFINITION, getIndexAtAddressForSlave(ns_slaveModeDefines::TIMEOUT));
	CHECK_ADDRESS(currentVal);
	QString oldVal = currentVal->value();
	if (currentVal->value() != lEdTimeoutSlaveRespond->text())							// Если значения не совпадают, обновляем
		currentVal->updateValue(lEdTimeoutSlaveRespond->text());

	bool isValid = isTimeoutValid();		// Проверяем сумму таймаутов со всех слейвов
	if (isValid == false) {					// Если проверка не прошла
		currentVal->updateValue(oldVal);	// Восстанавливаем старое значение в шаблон
		lEdTimeoutSlaveRespond->setText(ERR_STR_ERROR + ERR_STR_TIMEOUT_SUMM_EXCEED);	// Ошибку в форму
		lEdTimeoutSlaveRespond->setStyleSheet(ERR_QLINEEDIT_STYLE_SHEET);
		//MsgBox::error(ERR_STR_TIMEOUT_SUMM_EXCEED);	// Всплывающая ошибка
	}
	else {
		lEdTimeoutSlaveRespond->setStyleSheet(GOOD_QLINEEDIT_STYLE_SHEET);
	}
	
}



void TabSybusMasterIdentification::slotChangeComBoxCodeType(const QString &) {		// Узнать список кодов

	updateIdenAfterValueEntering();
}

void TabSybusMasterIdentification::slotChangeLEdVersion() {

	if (lEdProgramVersion->text().isEmpty()) {						// Если текста нет
		lEdProgramVersion->setStyleSheet(GOOD_QLINEEDIT_STYLE_SHEET);
		updateIdenAfterValueEntering();
		return;
	}

	bool isInt;
	uint64_t num = lEdProgramVersion->text().toULong(&isInt);

	if (!isInt || num > LIMIT_BYTE_NUMBER) {
		lEdProgramVersion->setStyleSheet(ERR_QLINEEDIT_STYLE_SHEET);
		lEdProgramVersion->setText(ERRORTEXT_NOT_IN_RANGE);
	}
	else {
		lEdProgramVersion->setStyleSheet(GOOD_QLINEEDIT_STYLE_SHEET);
		updateIdenAfterValueEntering();
	}
}

void TabSybusMasterIdentification::slotChangeLEdPhysicalAddress() {

	if (lEdPhysicalAddress->text().isEmpty()) {						// Если текста нет
		lEdPhysicalAddress->setStyleSheet(GOOD_QLINEEDIT_STYLE_SHEET);
		updateIdenAfterValueEntering();
		return;
	}

	bool isInt;
	uint64_t num = lEdPhysicalAddress->text().toULong(&isInt);

	if (!isInt || num > LIMIT_PHYSICAL_ADDRESS_MAX) {		// Если введено не число или вне диапазона		
		lEdPhysicalAddress->setStyleSheet(ERR_QLINEEDIT_STYLE_SHEET);
		lEdPhysicalAddress->setText(ERRORTEXT_NOT_IN_RANGE);		
	}
	else {
		lEdPhysicalAddress->setStyleSheet(GOOD_QLINEEDIT_STYLE_SHEET);
		updateIdenAfterValueEntering();
	}
}

void TabSybusMasterIdentification::slotChangeLEdSerialNumber() {

	if (lEdSerialNumber->text().isEmpty()) {						// Если текста нет
		lEdSerialNumber->setStyleSheet(GOOD_QLINEEDIT_STYLE_SHEET);
		updateIdenAfterValueEntering();
		return;		
	}

	bool isInt;
	uint64_t num = lEdSerialNumber->text().toULongLong(&isInt);

	if (!isInt || num > LIMIT_4BYTE_NUMBER) {		
		lEdSerialNumber->setStyleSheet(ERR_QLINEEDIT_STYLE_SHEET);
		lEdSerialNumber->setText(ERRORTEXT_NOT_IN_RANGE);		
	}
	else {
		lEdSerialNumber->setStyleSheet(GOOD_QLINEEDIT_STYLE_SHEET);
		updateIdenAfterValueEntering();
	}
}

void TabSybusMasterIdentification::slotChangeLEdProgramCode() {

	if (lEdProgramCode->text().isEmpty()) {						// Если текста нет
		lEdProgramCode->setStyleSheet(GOOD_QLINEEDIT_STYLE_SHEET);
		updateIdenAfterValueEntering();
		return;
	}

	bool isInt;
	uint64_t num = lEdProgramCode->text().toULong(&isInt);

	if (!isInt || num > LIMIT_2BYTE_NUMBER) {			   		
		lEdProgramCode->setStyleSheet(ERR_QLINEEDIT_STYLE_SHEET);
		lEdProgramCode->setText(ERRORTEXT_NOT_IN_RANGE);		
	}
	else {
		lEdProgramCode->setStyleSheet(GOOD_QLINEEDIT_STYLE_SHEET);
		updateIdenAfterValueEntering();
	}
}

void TabSybusMasterIdentification::slotChangeLEdLogicalAddress() {

	if (lEdLogicalAddress->text().isEmpty()) {						// Если текста нет
		lEdLogicalAddress->setStyleSheet(GOOD_QLINEEDIT_STYLE_SHEET);
		updateIdenAfterValueEntering();
		return;
	}

	bool isInt;
	uint64_t num = lEdLogicalAddress->text().toULong(&isInt);

	if (!isInt || num > LIMIT_BYTE_NUMBER) {				
		lEdLogicalAddress->setStyleSheet(ERR_QLINEEDIT_STYLE_SHEET);
		lEdLogicalAddress->setText(ERRORTEXT_NOT_IN_RANGE);		
	}
	else {
		lEdLogicalAddress->setStyleSheet(GOOD_QLINEEDIT_STYLE_SHEET);
		updateIdenAfterValueEntering();
	}
}

