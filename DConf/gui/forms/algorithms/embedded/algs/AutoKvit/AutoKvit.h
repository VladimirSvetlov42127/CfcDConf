#pragma once

#include <gui/forms/algorithms/embedded/IAlgorithm.h>

class AutoKvit : public IAlgorithm
{
public:
	AutoKvit();

	virtual QString name() const override;	
	virtual QWidget* createWidget() const override;

protected:
	virtual void fillReport(DcReportTable *table) const override;
};

ALGORITHMS_REGISTER(AutoKvit)
