#pragma once

#include <gui/forms/algorithms/embedded/algs/Board_Algorithm.h>

class NBLNSAlgorithm : public Board_Algorithm
{
public:
	NBLNSAlgorithm();

	virtual QString name() const override { return "Небаланс"; }
	virtual QWidget* createWidget() const override;

protected:
	virtual void fillReport(DcReportTable *table) const override;
};

ALGORITHMS_REGISTER(NBLNSAlgorithm)
