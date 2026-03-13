#pragma once

#include <gui/forms/algorithms/embedded/IAlgorithm.h>

class AMTZAlgorithm : public IAlgorithm
{
public:
	AMTZAlgorithm(bool Io);

	virtual QString name() const override;
	virtual QWidget* createWidget() const override;

protected:
	virtual bool checkForAvailable() const;
	virtual void fillReport(DcReportTable *table) const override;

private:
	bool m_Io;
};

ALGORITHMS_REGISTER_ARG(AMTZAlgorithm, false)
ALGORITHMS_REGISTER_ARG(AMTZAlgorithm, true)
