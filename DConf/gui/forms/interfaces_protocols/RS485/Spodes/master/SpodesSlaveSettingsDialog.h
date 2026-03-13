#pragma once

#include <qdialog.h>

#include <file_managers/DcSpodesMasterSlavesFileManager.h>

class DcController;
class ParameterElement;
class SpodesSlaveWidget;

class SpodesSlaveSettingsDialog : public QDialog
{
public:
	SpodesSlaveSettingsDialog(uint16_t portIdx, DcController *controller, QWidget *parent = nullptr);
	~SpodesSlaveSettingsDialog();

private slots:
	void onSlaveComboBoxIndexChanged(int idx);
	void onSaveButton();

private:	
	bool writeToSlaveParam(const QByteArray &data, uint8_t slaveGlobalIdx);
	void loadSlaves();
    ParameterElement* getParam(uint32_t addr, uint32_t idx, bool showError = true);

private:
	uint16_t m_portIdx;
	DcController *m_controller;
	QWidget* m_currentSlaveWidget;
	QList<SpodesSlaveWidget*> m_slaveWidgets;

	DcSpodesMasterSlavesFileManager m_fileManager;
};
