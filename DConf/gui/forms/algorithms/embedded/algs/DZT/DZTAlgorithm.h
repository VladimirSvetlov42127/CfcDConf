#pragma once

#include <gui/forms/algorithms/embedded/algs/Board_Algorithm.h>

class DZTAlgorithm : public Board_Algorithm
{
public:
	DZTAlgorithm();

	virtual QString name() const override { return "ДЗТ"; }
	virtual QWidget* createWidget() const override;

protected:
	virtual void fillReport(DcReportTable *table) const override;
};

ALGORITHMS_REGISTER(DZTAlgorithm)
