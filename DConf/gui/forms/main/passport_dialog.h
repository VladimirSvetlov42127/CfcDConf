#pragma once

#include <qdialog.h>

#include "device_operations/config_passport_operation.h"

class QTreeWidget;

class DcPassportDialog : public QDialog
{
	Q_OBJECT

public:
	DcPassportDialog(DcController *controller, QWidget *parent = nullptr);

    ConfigPassportOperation::Structure structure() const { return m_ps; }

public slots:
	void onOkButton();

private:
	QTreeWidget *m_treeWidget;
    ConfigPassportOperation::Structure m_ps;
};
