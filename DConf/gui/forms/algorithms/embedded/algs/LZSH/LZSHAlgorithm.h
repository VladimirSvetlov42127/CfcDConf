#pragma once

#include <gui/forms/algorithms/embedded/algs/Board_Algorithm.h>

class LZSHAlgorithm : public Board_Algorithm
{
public:
	LZSHAlgorithm();

	virtual QString name() const override { return "ЛЗШ"; }
	virtual QWidget* createWidget() const override;

protected:
	virtual void fillReport(DcReportTable *table) const override;
};

ALGORITHMS_REGISTER(LZSHAlgorithm)
