#pragma once

#include <qwidget.h>

#include "GoosePublisher.h"

class QLineEdit;
class QCheckBox;

namespace Dpc::Gui
{
	class ComboBox;
	class TableView;
}

class DcController;
class ConnectionDiscretWidget;

class InputGooseWidget : public QWidget
{
	Q_OBJECT
public:
	InputGooseWidget(DcController *controller, int interfaceIdx, int deviceIdx, QWidget *parent = nullptr);
	~InputGooseWidget();

	void setPublisher(const GoosePublisher &pub);
	GoosePublisher publisher() const { return m_publisher; }

	QList<GooseSubscriberEntryCfg> incomingDataList() const;
	void setIncomingDataList(const QList<GooseSubscriberEntryCfg> &list);

	int lgosMode() const;
	bool isActive() const;
	bool isControlAfterRestart() const;
	bool isControlAfterReconnection() const;
	int ratioTimeAllowedToLive() const;

	void setLgosMode(int mode);
	void setActive(bool active);
	void setControlAfterRestart(bool control);
	void setControlAfterReconnection(bool control);
	void setRatioTimeAllowedToLive(int ratio);

signals:
	void selectPublisher();

private slots:
	void onClearButton();

protected:
	void showEvent(QShowEvent *event) override;

private:
	GoosePublisher m_publisher;

	QLineEdit *m_publisherEdit;
	QLineEdit *m_macAddressEdit;
	QLineEdit *m_vlanEdit;
	QLineEdit *m_priorityVlanEdit;
	QLineEdit *m_appIdEdit;
	QLineEdit *m_confRevEdit;
	QLineEdit *m_gooseIdEdit;
	QLineEdit *m_goCbRefEdit;
	QLineEdit *m_goDatSetEdit;

	Dpc::Gui::TableView *m_dataSetView;
	Dpc::Gui::TableView *m_incomingDataView;

	Dpc::Gui::ComboBox *m_lgosComboBox;
	QCheckBox *m_activateCheckBox;
	QCheckBox *m_controlAfterRestartCheckBox;
	QCheckBox *m_controlAfterReconnectionCheckBox;
	Dpc::Gui::ComboBox *m_ratioTimeAllowedToLiveComboBox;

	ConnectionDiscretWidget *m_connectionDiscretWidget;
};