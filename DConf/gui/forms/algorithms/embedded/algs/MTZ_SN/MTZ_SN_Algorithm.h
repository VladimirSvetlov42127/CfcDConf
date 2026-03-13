#pragma once

#include <gui/forms/algorithms/embedded/algs/Board_Algorithm.h>

class MTZ_SN_Algorithm : public Board_Algorithm
{
public:
	MTZ_SN_Algorithm();

	virtual QString name() const override { return "МТЗ по СН"; }
	virtual QWidget* createWidget() const override;

protected:
	virtual void fillReport(DcReportTable *table) const override;
};

ALGORITHMS_REGISTER(MTZ_SN_Algorithm)
