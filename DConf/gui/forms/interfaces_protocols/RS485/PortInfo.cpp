#include "PortInfo.h"

PortInfo::PortInfo(DcController *controller)
	: m_controller(controller)
{
}

PortInfo::~PortInfo()
{
}

int32_t PortInfo::getPortCount()
{
	if (!m_controller)
		return -1;

    auto port = m_controller->paramsRegistry().parameter(SP_USARTPRTPAR_BYTE);
	if (!port)
		return -2;
	   	 
    return port->profilesCount();
}

int32_t PortInfo::getSummSlaveCountFromAllPorts()
{
	if (!m_controller)
		return -1;

    auto slave = m_controller->paramsRegistry().parameter(SP_USARTEXTNET_DEFINITION);
	if (!slave)
		return -2;

    return slave->profilesCount();
}

int32_t PortInfo::getSlaveCountForPort(uint16_t portNumber)
{
	// На данный момент на всех портах одинаковое кол-во слейвов

	if (!m_controller)
		return -1;

    auto port = m_controller->paramsRegistry().parameter(SP_USARTEXTNET_DEFINITION);
	if (!port)
		return -2;

    int32_t slaveCount = port->profilesCount();	// Кол-во слейвов на всех портах
	int32_t portCount = getPortCount();		// Кол-во портов

	if (portCount <= 0)
		return -3;

	if (slaveCount % portCount != 0)	// Если не делится нацело
		return -4;
	
	return slaveCount / portCount;
}

int32_t PortInfo::getSlaveCountForEachPort()
{
	return getSlaveCountForPort(0);
}

int32_t PortInfo::getSlaveConfigSize()
{
	if (!m_controller)
		return -1;

    auto slave = m_controller->paramsRegistry().parameter(SP_USARTEXTNET_DEFINITION);
	if (!slave)
		return -2;

    return slave->elementsCount();
}

int32_t PortInfo::getMaxTypeCountForAllPorts(ModbusMaster_ns::channelType_t type)
{
	if (!m_controller)
		return -1;

	int32_t count = -1;
	uint32_t address = 0;
	uint32_t index = 0;
	switch (type)
	{
	case ModbusMaster_ns::AinsInd:
		return m_controller->getSignalList(DEF_SIG_TYPE_ANALOG, DEF_SIG_SUBTYPE_REMOTE).size();
	case ModbusMaster_ns::DinsInd:
		return m_controller->getSignalList(DEF_SIG_TYPE_DISCRETE, DEF_SIG_SUBTYPE_REMOTE).size();
	case ModbusMaster_ns::CinsInd:
		return m_controller->getSignalList(DEF_SIG_TYPE_COUNTER, DEF_SIG_SUBTYPE_REMOTE).size();
	case ModbusMaster_ns::RAoutsInd:	// НЕ РЕАЛИЗОВАНО
	case ModbusMaster_ns::RDoutsInd:	// НЕ РЕАЛИЗОВАНО
	case ModbusMaster_ns::WAoutsInd:	// НЕ РЕАЛИЗОВАНО
	case ModbusMaster_ns::WDoutsInd:	// НЕ РЕАЛИЗОВАНО
	default:
		return -1;
	}

	return -2;
}

int32_t PortInfo::getCurrentTypeCountForAllPorts(ModbusMaster_ns::channelType_t type)
{
	int32_t count = -1;

	uint32_t lowBytePos = 0;
	uint32_t highBytePos = 0;

	switch (type)
	{
	case ModbusMaster_ns::AinsInd:
		lowBytePos = 3;
		highBytePos = 4;
		break;
	case ModbusMaster_ns::DinsInd:
		lowBytePos = 5;
		highBytePos = 6;
		break;
	case ModbusMaster_ns::CinsInd:
		lowBytePos = 7;
		highBytePos = 8;
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

	return getCountFromAllSlaveByPositions(lowBytePos, highBytePos);
}





int32_t PortInfo::getCountFromAllSlaveByPositions(uint32_t lowBytePos, uint32_t highButePos)
{
	if (lowBytePos == highButePos)
		return -1;

	int32_t slaveCount = getSummSlaveCountFromAllPorts();
	if (slaveCount < 0)
		return -1;

	int32_t slaveDisplacement = 0;
	uint32_t allCount = 0;

	for (int i = 0; i < slaveCount; i++) {

		slaveDisplacement = i * PROFILE_SIZE;

        auto slaveInfoLowByte = m_controller->paramsRegistry().element(SP_USARTEXTNET_DEFINITION, slaveDisplacement + lowBytePos);
		if (!slaveInfoLowByte)
			return -2;

        auto slaveInfoHighByte = m_controller->paramsRegistry().element(SP_USARTEXTNET_DEFINITION, slaveDisplacement + highButePos);
		if (!slaveInfoHighByte)
			return -3;

		uint16_t count = slaveInfoLowByte->value().toInt() + (slaveInfoHighByte->value().toInt() << 8);

		allCount += count;
	}

	return allCount;
}
