#pragma once

#include <gui/forms/interfaces_protocols/RS485/DcRs485BaseProtocol.h>

class DcRs485SybusProtocol : public DcRs485BaseProtocol
{
public:
	DcRs485SybusProtocol(DcController *device, int portIdx);

	virtual void fillReport(DcIConfigReport *report, DcReportTable *main) override;
	int currentFormat() const;
};


class DcRs485SybusProtocolWidget : public DcRs485BaseProtocolWidget
{
	Q_OBJECT
public:
	DcRs485SybusProtocolWidget(DcRs485SybusProtocol *protocol, QWidget *parent = nullptr);
};