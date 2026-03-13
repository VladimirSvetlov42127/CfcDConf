#pragma once

#include <gui/forms/interfaces_protocols/RS485/IEC/101/DcRs485Iec101Protocol.h>

class DcRs485Iec101SlaveProtocol : public DcRs485Iec101Protocol
{
public:
	DcRs485Iec101SlaveProtocol(DcController *device, int portIdx);

	virtual void fillReport(DcIConfigReport *report, DcReportTable *main) override;
};

class DcRs485Iec101SlaveProtocolWidget : public DcRs485Iec101ProtocolWidget
{
	Q_OBJECT
public:
	DcRs485Iec101SlaveProtocolWidget(DcRs485Iec101SlaveProtocol *protocol, QWidget *parent = nullptr);

private:
	CheckParamEditor *m_differentFromAsduEditor;
};