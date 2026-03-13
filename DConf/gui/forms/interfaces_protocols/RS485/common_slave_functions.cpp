#include "common_slave_functions.h"


#include <gui/forms/interfaces_protocols/RS485/PortInfo.h>
#include <file_managers/DcModbusMasterSlavesFileManager.h>
#include <gui/forms/interfaces_protocols/RS485/Modbus/master/ModbusSlaveFileSaver.h>


namespace {

}

bool resetUnusedSlaves(DcController *controller, uint16_t portNumber, uint16_t activeSlaveCount)
{
	if (!controller)
		return false;

	PortInfo info(controller);

	int32_t slaveForAllPorts = info.getSummSlaveCountFromAllPorts();
	int32_t slaveForPort = info.getSlaveCountForEachPort();

	if (slaveForAllPorts < 0 || slaveForPort < 0)
		return false;

	// Все описания слейвов для всех портов хранятся в одном параметре
	uint8_t inactiveSlaveIndex = activeSlaveCount; // Нумерация от нуля

	while (inactiveSlaveIndex < slaveForPort) {

		QString filename = DcModbusMasterSlavesFileManager(controller).fileNameFor(portNumber, inactiveSlaveIndex);
		if (!filename.isEmpty()) {
			if (!ModbusSlaveFileSaver::removeSlaveCfgFile(controller, filename))
				return false;
		}

		if (!cleanSlaveConfig(controller, portNumber, inactiveSlaveIndex))
			return false;

		inactiveSlaveIndex++;	// Переходим к следующему индексу слейва
	}

	return true;
}

bool cleanSlaveConfig(DcController *controller, uint16_t portId, uint16_t slaveId)
{
	PortInfo info(controller);
	uint16_t startSlaveConfig = portId * PROFILE_SIZE * info.getSlaveCountForPort(portId) + slaveId * PROFILE_SIZE;

    auto startParam = controller->paramsRegistry().parameter(SP_USARTEXTNET_DEFINITION);
	if (!startParam)
		return false;

    uint16_t slaveConfigsize = startParam->elementsCount();
	for (int i = startSlaveConfig; i < startSlaveConfig + slaveConfigsize; i++) {	// Проходим по всем параметрам слейва
        auto param = controller->paramsRegistry().element(SP_USARTEXTNET_DEFINITION, i);
		if (!param)
			return false;

		param->updateValue(QString::number(0));	// Обнуляем значения
	}

	return true;
}
