#pragma once

#include <gui/forms/algorithms/embedded/IAlgorithm.h>

class BasePSCAlgorithm : public IAlgorithm
{
public:
	using IAlgorithm::IAlgorithm;
	virtual bool checkForAvailable() const override;
};
