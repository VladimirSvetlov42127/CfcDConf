#pragma once

#include <gui/forms/DcFormFactory.h>

#include <dpc/gui/delegates/ComboBoxDelegate.h>

#include <gui/forms/interfaces_protocols/IEC-61850/IED.h>
#include <file_managers/DcIec61850FileManager.h>

class QPushButton;
class QTabWidget;
class QLineEdit;
class QGroupBox;
class QTreeView;
class QGridLayout;
class QModelIndex;
class QTableView;
class QComboBox;

class IEC61850Configuration;
class InputGooseWidget;

class Dc61850Form : public DcForm
{
	Q_OBJECT

public:
	Dc61850Form(DcController *controller);
	~Dc61850Form();

	static bool isAvailableFor(DcController *controller);
	static void fillReport(DcIConfigReport *report);

private slots:
	void onLoadIcdFileButton();
	void onSaveConfigButton();
	void onExportCidButton();
	void onSettingsViewCustomContextMenuRequested(const QPoint &pos);
	void onSettingsViewSelected(const QModelIndex &current, const QModelIndex &previous);
	void onGooseComboBoxIndexChanged(int idx);
	void onGooseSelectPublisher();
	void onSvComboBoxChanged();
	void onSvModelDataChanged(const QModelIndex &index);

private:
	bool updateParamValue(int32_t addr, int32_t index, const QString &value);
	void loadConfigFromFile(const QString &fileName);
	void setEnableEditing(bool enable);
	bool saveConfig();
	bool saveConfigParams();
	bool isConfigValid() const;
	bool loadInputGooseSettings();
	bool saveInputGooseSettings();
	bool saveInputGooseParams();
	void clearSelectedItemWidget();
	bool checkJavaInstalled();
	void updateSVControlsEditor(const QList<SVControlPtr>& svList);

private:
	IEC61850Configuration *m_configuration;

	QPushButton *m_saveConfigButton;
	QPushButton *m_exportCidButton;
	QTabWidget *m_mainTabWidget;
	QWidget *m_communicationTab;
	QWidget *m_iedTab;
	QWidget *m_svTab;
	QGridLayout *m_inputGooseLayout;
	QLineEdit *m_iedNameEdit;
	QTreeView *m_dataModelView;
	QTreeView *m_settingsView;
	QWidget   *m_selectedItemWidget;
	QList<InputGooseWidget*> m_inputGooseWidgets;
	QWidget *m_currentGooseWidget;
	QTableView* m_gseTableView;
	QTableView* m_svTableView;
	QComboBox* m_svComboBox;
	Dpc::Gui::ComboBoxDelegate* m_nofAsduDelegate;
	Dpc::Gui::ComboBoxDelegate* m_smpRateDelegate;

	DcIec61850FileManager m_fileManager;

};

REGISTER_FORM(Dc61850Form, DcMenu::iec61850);