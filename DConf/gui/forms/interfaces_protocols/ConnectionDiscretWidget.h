#pragma once

#include <qwidget.h>

class DcController;
class ParameterElement;
class QComboBox;

class AlgService;

class ConnectionDiscretWidget : public QWidget
{
	Q_OBJECT

public:
	ConnectionDiscretWidget(DcController *controller, int interfaceIdx, int deviceIdx, const QString &deviceName, QWidget *parent = nullptr);
	~ConnectionDiscretWidget();

	void updateList();

private slots:
	void onComboBoxIndexChanged(int idx);

private:
    std::optional<uint8_t> subscriberProfile(uint8_t interfaceIdx, uint8_t deviceIdx) const;
    ParameterElement* getParam(uint32_t addr, uint32_t idx, bool showError = true) const;

private:
	DcController *m_controller;
	int m_interfaceIdx;
    int m_deviceIdx;
	int m_previousComboBoxIdx;
	QString m_deviceName;

    AlgService *m_connectionService = nullptr;

	QComboBox *m_comboBox;
};
