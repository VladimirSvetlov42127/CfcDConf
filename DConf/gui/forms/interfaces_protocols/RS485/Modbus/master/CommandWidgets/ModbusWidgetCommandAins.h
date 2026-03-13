#pragma once
#include <qlineedit.h>
#include <qcombobox.h>

#include <gui/forms/interfaces_protocols/RS485/Modbus/master/CommandWidgets/ModbusWidgetCommonCommand.h>
#include <gui/forms/interfaces_protocols/RS485/Modbus/master/Commands/ModbusCommandAins.h>

class ModbusWidgetCommandAins : public ModbusWidgetCommonCommand
{
	Q_OBJECT

public:

	enum {/*приходящая последовательность байт*/
		ModbusAinBytesOrder_BE = 0,	/*4 байта, начиная со старшего  4-3-2-1*/
		ModbusAinBytesOrder_LE,	/*4 байта, начиная с младшего  1-2-3-4*/
		ModbusAinBytesOrder_HLFL,	 /*4 байта, в последовательности  2-1-4-3*/
		ModbusAinBytesOrder_HLFB,  /*4 байта, в последовательности  3-4-1-2*/
		ModbusAinBytesOrder_16LE,   /*2 байта, в последовательности  1-2*/
		ModbusAinBytesOrder_16BE,   /*2 байта, в последовательности  2-1*/
		ModbusAinBytesOrder_Mercury3B,	/*3 байта, в последовательности  3-1-2*/
		ModbusAinBytesOrder_Mercury4B,	/*4 байта, в последовательности  3-4-1-2*/

		ModbusAinBytesOrder_Unknown,
	};

	enum {
		ModbusAinValAlg_Float = 0,	/*естественный float, преобразование не требуется*/
		ModbusAinValAlg_Uint32,	/*преобразование uint32_t во float */
		ModbusAinValAlg_Int32,	/* преобразование int32_t во float */
		ModbusAinValAlg_Uint16,	/* преобразование uint16_t во float */
		ModbusAinValAlg_Int16,	/* преобразование int16_t во float */
		ModbusAinValAlg_Mercury_P, /*формат счетчика Меркурий –знаковые  3 или4  байта для активной энергии*/
		ModbusAinValAlg_Mercury_Q,/* формат счетчика Меркурий –знаковые  3 или4  байта для реактивной энергии */

		ModbusAinValAlg_Unknown,
	};

	ModbusWidgetCommandAins(spCommandDescr_t command, bool showAllParams = false, QWidget *parent = Q_NULLPTR);
	~ModbusWidgetCommandAins();

private:
	QComboBox * m_comBoxDataOrder;
	QComboBox * m_comBoxUpdAlg;
	QLineEdit * m_lEdK0;
	QLineEdit * m_lEdK1;

	ModbusCommandAins *m_command;	//  Не удалять! Команда на конкретную команду, извлекается из spCommandDescr_t.

public slots:

	void slot_comBoxDataOrder_currentIndexChanged(int);
	void slot_comBoxUpdAlg_currentIndexChanged(int);

	void slot_lEdk0_editingFinished();
	void slot_lEdk1_editingFinished();

};
