#pragma once

#include <gui/forms/algorithms/embedded/algs/Board_Algorithm.h>

class MTZAlgorithm : public Board_Algorithm
{
public:
	enum Type {
		N1, 
		N2, 
		N3, 
		N4
	};

	MTZAlgorithm(Type t);

	virtual QString name() const override;	
	virtual QWidget* createWidget() const override;

protected:
	virtual bool checkForAvailable() const override;
	virtual void fillReport(DcReportTable *table) const override;

private:
	Type m_type;
};

ALGORITHMS_REGISTER_ARG(MTZAlgorithm, MTZAlgorithm::N1)
ALGORITHMS_REGISTER_ARG(MTZAlgorithm, MTZAlgorithm::N2)
ALGORITHMS_REGISTER_ARG(MTZAlgorithm, MTZAlgorithm::N3)
ALGORITHMS_REGISTER_ARG(MTZAlgorithm, MTZAlgorithm::N4)
