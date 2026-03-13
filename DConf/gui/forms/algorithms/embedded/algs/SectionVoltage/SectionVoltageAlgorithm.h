#pragma once

#include <gui/forms/algorithms/embedded/algs/Board_Algorithm.h>

class SectionVoltageAlgorithm : public Board_Algorithm
{
public:
	SectionVoltageAlgorithm();

	virtual QString name() const override { return "Напряжение секции"; }
	virtual QWidget* createWidget() const override;

protected:
	virtual void fillReport(DcReportTable *table) const override;
};

ALGORITHMS_REGISTER(SectionVoltageAlgorithm)
