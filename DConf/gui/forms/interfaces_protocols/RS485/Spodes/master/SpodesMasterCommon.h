#pragma once

#include <qbytearray.h>

namespace SpodesMaster
{
	enum SlaveDefinitionModeBits
	{
		ActivateBit = 0,
		InFileBit = 7
	};

#pragma pack(push, 1)
	struct SlaveDefinition
	{
		SlaveDefinition(bool inFile = false);
		SlaveDefinition(const QByteArray &buff);

		uint8_t mode;
		uint8_t timeout;
		uint16_t logAddr;
		uint16_t phisAddr;
		uint8_t addrSize;
		uint16_t ainsCount;
		uint16_t dinsCount;
		uint16_t cinsCount;
		uint16_t rAoutsCount;
		uint16_t rDoutsCount;
		uint16_t wAoutsCount;
		uint16_t wDoutsCount;

		bool isActive() const;
		bool isInFile() const;

		void setActive(bool active);
		void setInFile(bool inFile);

		QByteArray data() const;
		bool operator!=(const SlaveDefinition &o) const;
	};
#pragma pack(pop)

}