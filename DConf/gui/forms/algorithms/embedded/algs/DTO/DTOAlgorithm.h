#pragma once

#include <gui/forms/algorithms/embedded/algs/Board_Algorithm.h>

class DTOAlgorithm : public Board_Algorithm
{
public:
	DTOAlgorithm();

	virtual QString name() const override { return "ДТО"; }
	virtual QWidget* createWidget() const override;

protected:
	virtual void fillReport(DcReportTable *table) const override;
};

ALGORITHMS_REGISTER(DTOAlgorithm)
