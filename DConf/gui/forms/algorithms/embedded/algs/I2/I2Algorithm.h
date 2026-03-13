#pragma once

#include <gui/forms/algorithms/embedded/algs/Board_Algorithm.h>

class I2Algorithm : public Board_Algorithm
{
public:
	I2Algorithm();

	virtual QString name() const override { return "ТЗОП"; }
	virtual QWidget* createWidget() const override;

protected:
	virtual void fillReport(DcReportTable *table) const override;
};

ALGORITHMS_REGISTER(I2Algorithm)
