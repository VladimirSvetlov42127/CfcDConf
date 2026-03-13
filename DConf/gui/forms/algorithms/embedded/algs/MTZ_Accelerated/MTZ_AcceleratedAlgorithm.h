#pragma once

#include <gui/forms/algorithms/embedded/algs/Board_Algorithm.h>

class MTZ_AcceleratedAlgorithm : public Board_Algorithm
{
public:
	MTZ_AcceleratedAlgorithm();

	virtual QString name() const override { return "МТЗ ускоренная"; }
	virtual QWidget* createWidget() const override;

protected:
	virtual void fillReport(DcReportTable *table) const override;
};

ALGORITHMS_REGISTER(MTZ_AcceleratedAlgorithm)
