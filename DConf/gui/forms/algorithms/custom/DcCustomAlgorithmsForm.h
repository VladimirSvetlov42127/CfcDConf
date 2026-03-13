#pragma once

#include <gui/forms/DcFormFactory.h>

#include <file_managers/DcFlexLogicFileManager.h>

class QTableView;
class QPushButton;

class CfcAlgService;
class CfcAlgsModel;

class DcCustomAlgorithmsForm : public DcForm
{
	Q_OBJECT

public:
    DcCustomAlgorithmsForm(DcController *controller);
	~DcCustomAlgorithmsForm();

	static bool isAvailableFor(DcController *controller);
	static void fillReport(DcIConfigReport *report);

private slots:
	void update();

	void onAddButton();
	void onRemoveButton();
	void onCompileButton();
	void onViewDoubleClicked(const QModelIndex &index);
	void onEditorAboutToClose();

private:
    void editAlgorithm(CfcAlgService* cfcAlg);
    bool compileAlgorithm(CfcAlgService* cfcAlg);

private:
    CfcAlgsModel* m_model;
	QTableView *m_algsTableView;    
	QPushButton *m_addButton;
	QPushButton *m_removeButton;
	QPushButton *m_compileButton;

	DcFlexLogicFileManager m_fm;
};

REGISTER_FORM(DcCustomAlgorithmsForm, DcMenu::algorithms_custom);

