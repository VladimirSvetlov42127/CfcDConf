#include "SpodesMasterCommon.h"

namespace SpodesMaster 
{
	SlaveDefinition::SlaveDefinition(bool inFile) :
		mode(0), 
		timeout(0), 
		logAddr(0), 
		phisAddr(0), 
		addrSize(0), 
		ainsCount(0), 
		dinsCount(0), 
		cinsCount(0), 
		rAoutsCount(0), 
		rDoutsCount(0), 
		wAoutsCount(0), 
		wDoutsCount(0)
	{
		setInFile(inFile);
	}

	SlaveDefinition::SlaveDefinition(const QByteArray & buff)
	{
		memcpy(this, buff.data(), sizeof(SlaveDefinition));
	}

	bool SlaveDefinition::isActive() const
	{
		return mode & (1 << ActivateBit);
	}

	bool SlaveDefinition::isInFile() const
	{
		return mode & (1 << InFileBit);
	}

	void SlaveDefinition::setActive(bool active)
	{
		if (active)
			mode |= (1 << ActivateBit);
		else
			mode &= ~(1 << ActivateBit);
	}

	void SlaveDefinition::setInFile(bool inFile)
	{
		if (inFile)
			mode |= (1 << InFileBit);
		else
			mode &= ~(1 << InFileBit);
	}

	QByteArray SlaveDefinition::data() const
	{
		return QByteArray((const char*)this, sizeof(SlaveDefinition));
	}

	bool SlaveDefinition::operator!=(const SlaveDefinition & o) const
	{
		return !(mode == o.mode 
			&& timeout == o.timeout 
			&& logAddr == o.logAddr
			&& phisAddr == o.phisAddr
			&& addrSize == o.addrSize
			&& ainsCount == o.ainsCount
			&& dinsCount == o.dinsCount
			&& cinsCount == o.cinsCount
			&& rDoutsCount == o.rDoutsCount
			&& rAoutsCount == o.rAoutsCount
			&& wDoutsCount == o.wDoutsCount
			&& wAoutsCount == o.wAoutsCount);
	}
}