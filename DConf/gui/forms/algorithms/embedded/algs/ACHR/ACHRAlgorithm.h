#pragma once

#include <gui/forms/algorithms/embedded/algs/Board_Algorithm.h>

class ACHRAlgorithm : public Board_Algorithm
{
public:
	enum Type {
		T_1 = 0,
		T_2,
		T_C
	};

	ACHRAlgorithm(Type t);

	virtual QString name() const override { return m_name; }
	virtual QWidget* createWidget() const override;

protected:
	virtual void fillReport(DcReportTable *table) const override;

private:
	Type m_t;
	QString m_name;
};

ALGORITHMS_REGISTER_ARG(ACHRAlgorithm, ACHRAlgorithm::T_1)
ALGORITHMS_REGISTER_ARG(ACHRAlgorithm, ACHRAlgorithm::T_2)
ALGORITHMS_REGISTER_ARG(ACHRAlgorithm, ACHRAlgorithm::T_C)