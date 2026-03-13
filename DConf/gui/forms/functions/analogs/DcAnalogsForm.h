#pragma once

#include <dpc/gui/delegates/ComboBoxDelegate.h>

#include <gui/forms/DcFormFactory.h>

class DcAnalogsForm : public DcForm
{
	Q_OBJECT

public:
	DcAnalogsForm(DcController *controller);

	static bool isAvailableFor(DcController *controller);
	static void fillReport(DcIConfigReport *report);

private:
	QWidget* createConversionTab();
    QWidget* createComparisonTab();

	Dpc::Gui::ComboBoxDelegate* m_analogsDelegate;
	Dpc::Gui::ComboBoxDelegate* m_virtualAnalogsDelegate;
	Dpc::Gui::ComboBoxDelegate* m_virtualDiscretsDelegate;
};

REGISTER_FORM(DcAnalogsForm, DcMenu::analogs_functions);

