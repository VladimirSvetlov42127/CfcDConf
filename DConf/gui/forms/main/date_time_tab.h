#pragma once

#include "gui/forms/main/abstract_device_tab.h"

class QDateEdit;
class QTimeEdit;
class QComboBox;
class QLineEdit;

class DcController;

class DateTimeTab : public AbstractDeviceTab
{
	Q_OBJECT

public:
    DateTimeTab(DcController *device, QWidget *parent = nullptr);
    ~DateTimeTab();

    virtual void deviceOperationFinished(AbstractOperation *op) override;
    virtual QList<uint16_t> addrList() const override;

private:
	void setDateTime(const QDateTime& dt, const QVariant &offset);

private:
    QDateEdit *m_dateEdit;
    QTimeEdit *m_timeEdit;
    QComboBox *m_offsetComboBox;
    QLineEdit *m_protocolSync;
    QLineEdit *m_typeSync;
    DcController *m_device;
};
