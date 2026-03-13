#pragma once

#include <gui/forms/algorithms/embedded/algs/Board_Algorithm.h>

class KCTAlgorithm : public Board_Algorithm
{
public:
	KCTAlgorithm();

	virtual QString name() const override { return "КЦТ"; }
	virtual QWidget* createWidget() const override;

protected:
	virtual void fillReport(DcReportTable *table) const override;
};

ALGORITHMS_REGISTER(KCTAlgorithm)
