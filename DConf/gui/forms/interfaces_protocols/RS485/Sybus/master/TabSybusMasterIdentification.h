
#pragma once

#include <QObject>
#include "QVBoxLayout"
#include "QLineEdit"
#include "qcheckbox.h"
#include "qcombobox.h"
#include "qpair.h"

#include <data_model/dc_properties.h>
#include <dpc/gui/dialogs/msg_box/MsgBox.h>					// Всплывающий окна с ошибками и предупреждениями


class DcController;

class TabSybusMasterIdentification : public QObject
{
	Q_OBJECT
		
public:
    TabSybusMasterIdentification(DcController *controller, int portNumber, QObject *parent = nullptr);
	~TabSybusMasterIdentification();

	bool build(void);
	QWidget* formIdentification(void);
	int32_t error();
	QString errorMessage();

	// Функции, вызываемые из ведущего
	void updateSlaveList();			// Когда изменился выбор текущего слейва для отображения, обновляем все поля
	//QVector<QString>* checkSlaveSettingsIdentification();						// Проверяет, все ли параметры заполнены
	QVector<QString>* getErrorVectorOfStr();
	bool isAllParametersValid();	// Проверяет все параметры класса, соответствуют ли они диапазонам
	bool isTimeoutValid();

private:
	DcController	*m_controller;		// Указатель на модель контроллера	
	QString strErrorMessage;			// Сообщение с ошибкой для вывода
	int m_port_id = 0;					// Индекс порта
	int m_portNumber = 0;					// Номер порта
	int m_maxSlaveCount = 0;
	QWidget *widIdentification;			// Главный виджет
	int currentSlaveNumber;				// Номер текущего слейва

	// Настройка слейвов. Добавляются нв другую вкладку
	QComboBox *comBoxChooseSlave;
	QCheckBox *chBoxSlaveStateIsActive;
	QCheckBox *chBoxSlaveStateIsOldModel;
	QCheckBox *chBoxSlaveStateIsInpulseCommand;
	QCheckBox *chBoxSlaveStateIsEMZMmodel;
	QComboBox *comBoxSlaveStateModeOrCounter;
	QLineEdit *lEdTimeoutSlaveRespond;

	QComboBox *comBoxCodeType;
	QLineEdit *lEdProgramVersion;
	QLineEdit *lEdPhysicalAddress;
	QLineEdit *lEdSerialNumber;
	QLineEdit *lEdProgramCode;
	QLineEdit *lEdLogicalAddress;
	QWidget *m_connectionDiscret;
	
	QVector<QLineEdit*> vectorLineEditIdentification;				// Создаем вектор из полей ввода для идентификации
	QVector<QPair<QString, int>> vectorTypeCodes;					// Хранит строки для кодов типа 
	QVector<QString> vectorErrorStringsIdentification;				// Хранит строки с ошибками

	// Вспомогательные функции
	// Такие же как и в общем классе
	int getParameterCount(int address);
	bool checkIsValidValue(int param, int min, int max);			// Проверяет вхождение значения в интервал допустимых значений
	QString makeStrOfLimits(int min, int max);						// Выдает строку с диапазоном значений
	QString makeStrDefultAndLimitValue(int def, int min, int max);	// Выдает строку с значением по умолчанию и диапазоном мин/макс

	// Новые
	void blockIdentificatorSignals(bool);							// Блокирует сигналы всех объектов, относящихся к идентификации
	void updateParametersAfterSlaveChange();						// Обновление всех окон для текущего индекса слейва
	void updateCodes();
	bool updateIdenAfterValueEntering();							// Как только меняется значение, обновляем и записываем новый массив байт в модель
	int getIndexAtAddressForSlave(int);								// Облегчает получение смещения для адреса, если несколько слейвов (зависит от номера текущего выбранного)

	void cleanSlaveIdentificationInModel();							// Обнуляем байты идентификации в модели
	void cleanIdentificationForms();								// Обнуляет все поля формы в начальное состояние

    // Проверки
	bool isOneIdentificationParameterChoosen();						// Проверет, выбран ли хотя бы один из параметров идентификации


public slots: 
	// Слоты для настройки опций ведомого
	void slotChangeComBoxCurrentSlaveIndex(int);
	void slotChangeChBoxSlaveStateIsActive(int);					// Слоты настройки режима ведомого
	void slotChangeChBoxSlaveStateIsOldModel(int);
	void slotChangeChBoxSlaveStateIsInpulseCommand(int);
	void slotChangeChBoxSlaveStateIsEMZMmodel(int);
	void slotChangeComBoxSlaveStateModeOrCounter(const QString &);
	void slotChangeTEdTimeoutSlaveRespond();


// Слоты, отвечающие за запись данных. После записи откроеты доступ к чекбоксу
	void slotChangeComBoxCodeType(const QString &);		// Узнать список кодов
	void slotChangeLEdVersion();
	void slotChangeLEdPhysicalAddress();
	void slotChangeLEdSerialNumber();
	void slotChangeLEdProgramCode();
	void slotChangeLEdLogicalAddress();

};
