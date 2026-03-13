#pragma once

#include <gui/forms/algorithms/embedded/algs/Board_Algorithm.h>

class OZZ_PoAlgorithm : public Board_Algorithm
{
public:
	OZZ_PoAlgorithm(int idx);

	virtual QString name() const override { return m_name; }
	virtual QWidget* createWidget() const override;

protected:
	virtual void fillReport(DcReportTable *table) const override;

private:
	int m_idx;
	QString m_name;
};

ALGORITHMS_REGISTER_ARG(OZZ_PoAlgorithm, 0)
ALGORITHMS_REGISTER_ARG(OZZ_PoAlgorithm, 1)