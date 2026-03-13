#pragma once

#include <gui/forms/interfaces_protocols/RS485/DcRs485BaseProtocol.h>

class DcRs485IecProtocol : public DcRs485BaseProtocol
{
public:
	DcRs485IecProtocol(DcController *device, int portIdx);

	virtual void fillReport(DcIConfigReport *report, DcReportTable *main) override;
};

class DcRs485IecProtocolWidget : public DcRs485BaseProtocolWidget
{
	Q_OBJECT
public:
	DcRs485IecProtocolWidget(DcRs485IecProtocol *protocol, QWidget *parent = nullptr);
};