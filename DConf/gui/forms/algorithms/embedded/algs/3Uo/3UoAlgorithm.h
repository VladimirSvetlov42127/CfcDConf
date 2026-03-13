#pragma once

#include <gui/forms/algorithms/embedded/algs/Board_Algorithm.h>

class T3UoAlgorithm : public Board_Algorithm
{
public:
	T3UoAlgorithm();

	virtual QString name() const override { return "ОЗЗ по 3Uo"; }
	virtual QWidget* createWidget() const override;

protected:
	virtual bool checkForAvailable() const override;
	virtual void fillReport(DcReportTable *table) const override;
};

ALGORITHMS_REGISTER(T3UoAlgorithm)
