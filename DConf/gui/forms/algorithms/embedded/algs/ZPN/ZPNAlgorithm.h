#pragma once

#include <gui/forms/algorithms/embedded/algs/Board_Algorithm.h>

class ZPNAlgorithm : public Board_Algorithm
{
public:
	ZPNAlgorithm(int idx);

	virtual QString name() const override { return m_name; }
	virtual QWidget* createWidget() const override;

protected:
	virtual void fillReport(DcReportTable *table) const override;	

private:
	int m_idx;
	QString m_name;
};

ALGORITHMS_REGISTER_ARG(ZPNAlgorithm, 0)
ALGORITHMS_REGISTER_ARG(ZPNAlgorithm, 1)