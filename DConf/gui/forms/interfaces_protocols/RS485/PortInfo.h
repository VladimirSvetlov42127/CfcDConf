#pragma once

#include "data_model/dc_controller.h"
#include <gui/forms/interfaces_protocols/RS485/Modbus/master/Common/common_ModbusCommandDefines.h>

class PortInfo
{
public:
    explicit PortInfo(DcController *controller);
	~PortInfo();

	int32_t getPortCount();
	int32_t getSummSlaveCountFromAllPorts();
	int32_t getSlaveCountForPort(uint16_t portNumber);
	int32_t getSlaveCountForEachPort();
	int32_t getSlaveConfigSize();


	int32_t getMaxTypeCountForAllPorts(ModbusMaster_ns::channelType_t type);	// Для мобас мастера
	int32_t getCurrentTypeCountForAllPorts(ModbusMaster_ns::channelType_t type);	// Для мобас мастера

private:
	DcController *m_controller;

private:

	int32_t getCountFromAllSlaveByPositions(uint32_t lowBytePos, uint32_t highButePos);
};

