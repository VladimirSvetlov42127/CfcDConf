#pragma once

#include <gui/forms/algorithms/embedded/algs/DevicePSC/BasePSCAlgorithm.h>

class MTZ_PSCAlgorithm : public BasePSCAlgorithm
{
public:

	MTZ_PSCAlgorithm(bool Io);

	virtual QString name() const override;
	virtual QWidget* createWidget() const override;

private:
	bool m_Io;
};

ALGORITHMS_REGISTER_ARG(MTZ_PSCAlgorithm, false)
ALGORITHMS_REGISTER_ARG(MTZ_PSCAlgorithm, true)
