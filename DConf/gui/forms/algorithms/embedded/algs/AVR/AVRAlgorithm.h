#pragma once

#include <gui/forms/algorithms/embedded/algs/Board_Algorithm.h>

class AVRAlgorithm : public Board_Algorithm
{
public:
	AVRAlgorithm();

	virtual QString name() const override { return "АВР"; }
	virtual QWidget* createWidget() const override;

protected:
	virtual void fillReport(DcReportTable *table) const override;
};

ALGORITHMS_REGISTER(AVRAlgorithm)
