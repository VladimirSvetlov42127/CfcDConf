#pragma once

#include <gui/forms/algorithms/embedded/algs/Board_Algorithm.h>

class MTZ_NN_Algorithm : public Board_Algorithm
{
public:
	MTZ_NN_Algorithm();

	virtual QString name() const override { return "МТЗ по НН"; }
	virtual QWidget* createWidget() const override;

protected:
	virtual void fillReport(DcReportTable *table) const override;
};

ALGORITHMS_REGISTER(MTZ_NN_Algorithm)
