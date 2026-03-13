#pragma once

#include <gui/forms/algorithms/embedded/algs/Board_Algorithm.h>

class TO_VNAlgorithm : public Board_Algorithm
{
public:
	TO_VNAlgorithm();

	virtual QString name() const override { return "ТО по ВН"; }
	virtual QWidget* createWidget() const override;

protected:
	virtual void fillReport(DcReportTable *table) const override;
};

ALGORITHMS_REGISTER(TO_VNAlgorithm)
