#pragma once

#include <gui/forms/algorithms/embedded/algs/Board_Algorithm.h>

class BKIAlgorithm : public Board_Algorithm
{
public:
	BKIAlgorithm();

	virtual QString name() const override { return "БКИ"; }
	virtual QWidget* createWidget() const override;

protected:
	virtual void fillReport(DcReportTable *table) const override;
};

ALGORITHMS_REGISTER(BKIAlgorithm)
