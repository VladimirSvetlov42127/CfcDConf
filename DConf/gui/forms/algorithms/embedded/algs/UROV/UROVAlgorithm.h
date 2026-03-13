#pragma once

#include <gui/forms/algorithms/embedded/IAlgorithm.h>

class UROVAlgorithm : public IAlgorithm
{
public:
	UROVAlgorithm();

	virtual QString name() const override { return "УРОВ"; }
	virtual QWidget* createWidget() const override;

protected:
	virtual void fillReport(DcReportTable *table) const override;
};

ALGORITHMS_REGISTER(UROVAlgorithm)
