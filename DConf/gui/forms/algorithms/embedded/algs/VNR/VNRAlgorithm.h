#pragma once

#include <gui/forms/algorithms/embedded/algs/Board_Algorithm.h>

class VNRAlgorithm : public Board_Algorithm
{
public:
	VNRAlgorithm();

	virtual QString name() const override { return "ВНР"; }
	virtual QWidget* createWidget() const override;

protected:
	virtual void fillReport(DcReportTable *table) const override;
};

ALGORITHMS_REGISTER(VNRAlgorithm)
