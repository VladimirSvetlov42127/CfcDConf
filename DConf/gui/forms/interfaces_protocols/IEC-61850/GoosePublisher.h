#pragma once

#include "IEC61850Configuration.h"

#pragma pack(push, 1)
struct GooseSubscriberEntryCfg
{
	GooseSubscriberEntryCfg(uint32_t value = 0xFFFFFFFF);
	uint32_t tagNum : 12;
	uint32_t tagType : 4;
	uint32_t intAddr : 12;
	uint32_t intAddrLn : 4;

	uint32_t toUint() const;
};
#pragma pack(pop)

struct GoosePublisher
{
	QString fileName;
	IEC61850ConfigurationPtr config;
	IEDPtr ied;
	LDevicePtr device;
	LNPtr ln;
	GSEControlPtr gseControl;
};