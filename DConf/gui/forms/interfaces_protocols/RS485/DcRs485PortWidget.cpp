#include "DcRs485PortWidget.h"

#include <data_model/dc_controller.h>
#include <report/DcIConfigReport.h>

#include <gui/editors/EditorsManager.h>
#include <gui/forms/interfaces_protocols/RS485/Sybus/slave/DcRs485SybusSlaveProtocol.h>
#include <gui/forms/interfaces_protocols/RS485/Sybus/master/DcRs485SybusMasterProtocol.h>
#include <gui/forms/interfaces_protocols/RS485/Modbus/slave/DcRs485ModbusSlaveProtocol.h>
#include <gui/forms/interfaces_protocols/RS485/Modbus/master/DcRs485ModbusMasterProtocol.h>
#include <gui/forms/interfaces_protocols/RS485/Spodes/slave/DcRs485SpodesSlaveProtocol.h>
#include <gui/forms/interfaces_protocols/RS485/Spodes/master/DcRs485SpodesMasterProtocol.h>
#include <gui/forms/interfaces_protocols/RS485/IEC/101/slave/DcRs485Iec101SlaveProtocol.h>
#include <gui/forms/interfaces_protocols/RS485/IEC/103/slave/DcRs485Iec103SlaveProtocol.h>
#include <gui/forms/interfaces_protocols/RS485/TransparentChannel/dc_rs485_transparent_protocol.h>

namespace {
	const ListEditorContainer g_ProtocolsList = {
		{ "Не установлено", DcController::NotSet },
		{ "Sybus ведомый", DcController::SybusSlaveProtocol },
		{ "Sybus ведущий", DcController::SybusMasterProtocol },
		{ "Modbus ведомый", DcController::ModbusSlaveProtocol },
		{ "Modbus ведущий", DcController::ModbusMasterPrototcol },
		{ "МЭК 60870-5-101 ведомый", DcController::Iec101SlaveProtocol },
		{ "МЭК 60870-5-101 ведущий", DcController::Iec101MasterProtocol },
		{ "МЭК 60870-5-103 ведомый", DcController::Iec103SlaveProtocol },
		{ "СПОДЭС ведомый", DcController::SpodesSlaveProtocol },
		{ "СПОДЭС ведущий", DcController::SpodesMasterProtocol },
		{ "Hayes modem ведущий", DcController::HayesModemMasterProtocol },
		{ "Модем Нанотрон", DcController::ModemNanotronProtocol },
        { "Прозрачный канал", DcController::ComPortProtocol },
	};

	DcRs485BaseProtocol* createProtocol(uint protocol, DcController *device, int portIdx)
	{
		switch (protocol)
		{
		case DcController::SybusSlaveProtocol: return new DcRs485SybusSlaveProtocol(device, portIdx);
		case DcController::SybusMasterProtocol: return new DcRs485SybusMasterProtocol(device, portIdx);
		case DcController::ModbusSlaveProtocol: return new DcRs485ModbusSlaveProtocol(device, portIdx);
		case DcController::ModbusMasterPrototcol: return new DcRs485ModbusMasterProtocol(device, portIdx);
		case DcController::SpodesSlaveProtocol: return new DcRs485SpodesSlaveProtocol(device, portIdx);
		case DcController::SpodesMasterProtocol: return new DcRs485SpodesMasterProtocol(device, portIdx);
		case DcController::Iec101SlaveProtocol: return new DcRs485Iec101SlaveProtocol(device, portIdx);
		case DcController::Iec103SlaveProtocol: return new DcRs485Iec103SlaveProtocol(device, portIdx);
        case DcController::ComPortProtocol: return new DcRs485TransparentProtocol(device, portIdx);
		default: return nullptr;
		}
	}

	DcRs485BaseProtocolWidget* createProtocolWidget(uint protocol, DcController *device, int portIdx)
	{
		switch (protocol)
		{
		case DcController::SybusSlaveProtocol: return new DcRs485SybusSlaveProtocolWidget(new DcRs485SybusSlaveProtocol(device, portIdx));
		case DcController::SybusMasterProtocol: return new DcRs485SybusMasterProtocolWidget(new DcRs485SybusMasterProtocol(device, portIdx));
		case DcController::ModbusSlaveProtocol: return new DcRs485ModbusSlaveProtocolWidget(new DcRs485ModbusSlaveProtocol(device, portIdx));
		case DcController::ModbusMasterPrototcol: return new DcRs485ModbusMasterProtocolWidget(new DcRs485ModbusMasterProtocol(device, portIdx));
		case DcController::SpodesSlaveProtocol: return new DcRs485SpodesSlaveProtocolWidget(new DcRs485SpodesSlaveProtocol(device, portIdx));
		case DcController::SpodesMasterProtocol: return new DcRs485SpodesMasterProtocolWidget(new DcRs485SpodesMasterProtocol(device, portIdx));
		case DcController::Iec101SlaveProtocol: return new DcRs485Iec101SlaveProtocolWidget(new DcRs485Iec101SlaveProtocol(device, portIdx));
		case DcController::Iec103SlaveProtocol: return new DcRs485Iec103SlaveProtocolWidget(new DcRs485Iec103SlaveProtocol(device, portIdx));
        case DcController::ComPortProtocol: return new DcRs485TransparentProtocolWidget(new DcRs485TransparentProtocol(device, portIdx));
		default: return nullptr;
		}
	}

	ListEditorContainer supportedProtocols(DcController* contr) 
	{
		auto protocolList = g_ProtocolsList;
        if (auto param = contr->paramsRegistry().parameter(SP_PROTRS485_LIST); param) {
            for (int i = 0; i < param->elementsCount(); i++) {
                if (!contr->paramsRegistry().element(SP_PROTRS485_LIST, i)->value().toUInt())
					protocolList.exclude({ i + 1 });
			}
		}

		return protocolList;
	}
}

namespace Text {
	const QString Protocol = "Протокол";
}


DcRs485PortWidget::DcRs485PortWidget(DcController *controller, int portIdx, QWidget *parent) :
	QWidget(parent)
{
	auto wCreator = [=](const QVariant &data) {
		return createProtocolWidget(data.toUInt(), controller, portIdx);
	};

	QGridLayout *layout = new QGridLayout(this);
	EditorsManager mg(controller, layout);

	auto editor = mg.addListEditor(SP_USARTPRTPAR_BYTE, portIdx * PROFILE_SIZE, Text::Protocol, supportedProtocols(controller));
	mg.addWidgetCreatorLayout(editor, wCreator);

	layout->setRowStretch(layout->rowCount() - 1, 1);
	layout->setColumnStretch(2, 1);
}

void DcRs485PortWidget::fillReport(DcIConfigReport * report, int portIdx)
{
	auto device = report->device();
    auto protocolParam = device->paramsRegistry().element(SP_USARTPRTPAR_BYTE, portIdx * PROFILE_SIZE);

	DcReportTable table(device);
	table.addRow(DcReportTable::ParamRecord(protocolParam, supportedProtocols(device).toHash(), Text::Protocol));
	auto protocol = std::shared_ptr<DcRs485BaseProtocol>(createProtocol(protocolParam->value().toUInt(), report->device(), portIdx));
	if (protocol)
		protocol->fillReport(report, &table);
	
	report->insertTable(table);
}
