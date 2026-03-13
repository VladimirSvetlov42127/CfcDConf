#pragma once

#include <gui/forms/algorithms/embedded/algs/Board_Algorithm.h>

class OZZ_HH_Algorithm : public Board_Algorithm
{
public:
	OZZ_HH_Algorithm();

	virtual QString name() const override { return "ОЗЗ по высшим гармоникам"; }
	virtual QWidget* createWidget() const override;

protected:
	virtual void fillReport(DcReportTable *table) const override;

private:
};

ALGORITHMS_REGISTER(OZZ_HH_Algorithm)