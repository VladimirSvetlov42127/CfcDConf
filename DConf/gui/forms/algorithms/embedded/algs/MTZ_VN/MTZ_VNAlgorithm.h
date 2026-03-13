#pragma once

#include <gui/forms/algorithms/embedded/algs/Board_Algorithm.h>

class MTZ_VNAlgorithm : public Board_Algorithm
{
public:
	MTZ_VNAlgorithm();

	virtual QString name() const override { return "МТЗ по ВН"; }
	virtual QWidget* createWidget() const override;

protected:
	virtual void fillReport(DcReportTable *table) const override;
};

ALGORITHMS_REGISTER(MTZ_VNAlgorithm)
