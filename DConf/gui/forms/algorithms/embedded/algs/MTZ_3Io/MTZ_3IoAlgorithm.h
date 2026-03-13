#pragma once

#include <gui/forms/algorithms/embedded/algs/Board_Algorithm.h>

class MTZ_3IoAlgorithm : public Board_Algorithm
{
public:
	MTZ_3IoAlgorithm(int idx);			

	virtual QString name() const override { return m_name; }
	virtual QWidget* createWidget() const override;

protected:
	virtual bool checkForAvailable() const override;
	virtual void fillReport(DcReportTable *table) const override;

private:
	int m_idx;
	QString m_name;
};

ALGORITHMS_REGISTER_ARG(MTZ_3IoAlgorithm, 0)
ALGORITHMS_REGISTER_ARG(MTZ_3IoAlgorithm, 1)
ALGORITHMS_REGISTER_ARG(MTZ_3IoAlgorithm, 2)