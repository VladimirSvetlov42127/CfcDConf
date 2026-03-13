#pragma once

#include <gui/forms/algorithms/embedded/algs/DevicePSC/BasePSCAlgorithm.h>

class AMTZ_PSCAlgorithm : public BasePSCAlgorithm
{
public:
	AMTZ_PSCAlgorithm(bool Io);

	virtual QString name() const override;
	virtual QWidget* createWidget() const override;

private:
	bool m_Io;
};

ALGORITHMS_REGISTER_ARG(AMTZ_PSCAlgorithm, false)
ALGORITHMS_REGISTER_ARG(AMTZ_PSCAlgorithm, true)
