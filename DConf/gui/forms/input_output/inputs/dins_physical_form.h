#ifndef __DINS_PHYSICAL_FORM_H__
#define __DINS_PHYSICAL_FORM_H__

#include "gui/forms/DcFormFactory.h"

namespace Dpc::Gui {
class TableView;
}

class DinsPhysicalForm : public DcForm
{
	Q_OBJECT

public:
    DinsPhysicalForm(DcController* controller);

    static bool isAvailableFor(DcController* controller);
	static void fillReport(DcIConfigReport* report);

private:
    Dpc::Gui::TableView* makeTableView(DcController* controller, DcBoard* board = nullptr);
};

REGISTER_FORM(DinsPhysicalForm, DcMenu::dins_physical);


#endif	//	__DINS_PHYSICAL_FORM_H__
