#pragma once

#include <gui/forms/algorithms/embedded/algs/DevicePSC/BasePSCAlgorithm.h>

class OZZ_PSCAlgorithm : public BasePSCAlgorithm
{
public:
	OZZ_PSCAlgorithm();

	virtual QString name() const override { return "ОЗЗ"; }
	virtual QWidget* createWidget() const override;
};

ALGORITHMS_REGISTER(OZZ_PSCAlgorithm)
