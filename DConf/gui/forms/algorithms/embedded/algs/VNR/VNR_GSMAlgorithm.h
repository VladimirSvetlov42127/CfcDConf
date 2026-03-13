#pragma once

#include <gui/forms/algorithms/embedded/IAlgorithm.h>

class VNR_GSMAlgorithm : public IAlgorithm
{
public:
	VNR_GSMAlgorithm();

	virtual QString name() const override { return "ВНР"; }
	virtual bool checkForAvailable() const override;
	virtual QWidget* createWidget() const override;
};

ALGORITHMS_REGISTER(VNR_GSMAlgorithm)
