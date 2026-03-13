#pragma once

#include <gui/forms/algorithms/embedded/IAlgorithm.h>

class APVAlgorithm : public IAlgorithm
{
public:
	APVAlgorithm();

	virtual QString name() const override { return "АПВ"; }
	virtual QWidget* createWidget() const override;

protected:
	virtual void fillReport(DcReportTable *table) const override;
};

ALGORITHMS_REGISTER(APVAlgorithm)
