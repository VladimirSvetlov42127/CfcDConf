#pragma once

#include <gui/forms/algorithms/embedded/IAlgorithm.h>

class AUVAlgorithm : public IAlgorithm
{
public:
	AUVAlgorithm();

	virtual QString name() const override { return "АУВ"; }
	virtual QWidget* createWidget() const override;

protected:
	virtual void fillReport(DcReportTable *table) const override;
};

ALGORITHMS_REGISTER(AUVAlgorithm)
