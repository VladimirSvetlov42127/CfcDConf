#ifndef DCRS485TRANSPARENTPROTOCOL_H
#define DCRS485TRANSPARENTPROTOCOL_H

#include <gui/forms/interfaces_protocols/RS485/DcRs485BaseProtocol.h>

class DcRs485TransparentProtocol : public DcRs485BaseProtocol
{
public:
    DcRs485TransparentProtocol(DcController *device, int portIdx);

    virtual void fillReport(DcIConfigReport *report, DcReportTable *main) override;
    int currentFormat() const;
};

class DcRs485TransparentProtocolWidget : public DcRs485BaseProtocolWidget
{
    Q_OBJECT
public:
    DcRs485TransparentProtocolWidget(DcRs485TransparentProtocol *protocol, QWidget *parent = nullptr);
};

#endif // DCRS485TRANSPARENTPROTOCOL_H
