#pragma once

#include <gui/forms/algorithms/embedded/algs/Board_Algorithm.h>

class U2Algorithm : public Board_Algorithm
{
public:
	U2Algorithm();

	virtual QString name() const override { return "ЗОФ"; }
	virtual QWidget* createWidget() const override;

protected:
	virtual void fillReport(DcReportTable *table) const override;
};

ALGORITHMS_REGISTER(U2Algorithm)
