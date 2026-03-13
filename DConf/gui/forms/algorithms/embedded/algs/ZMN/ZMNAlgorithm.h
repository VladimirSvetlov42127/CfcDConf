#pragma once

#include <gui/forms/algorithms/embedded/algs/Board_Algorithm.h>

class ZMNAlgorithm : public Board_Algorithm
{
public:
	ZMNAlgorithm(bool mks);	

	virtual QString name() const override { return m_name; }
	virtual QWidget* createWidget() const override;

protected:
	virtual void fillReport(DcReportTable *table) const override;

private:
	bool m_mks;
	QString m_name;
};

ALGORITHMS_REGISTER_ARG(ZMNAlgorithm, false)
ALGORITHMS_REGISTER_ARG(ZMNAlgorithm, true)