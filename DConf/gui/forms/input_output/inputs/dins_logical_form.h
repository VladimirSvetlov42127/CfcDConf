#ifndef __DINS_LOGICAL_FORM_H__
#define __DINS_LOGICAL_FORM_H__

#include <gui/forms/DcFormFactory.h>

class DinsLogicalForm : public DcForm
{
	Q_OBJECT

public:
	DinsLogicalForm(DcController* controller);
	static bool isAvailableFor(DcController* controller);
    static void fillReport(DcIConfigReport* report);
};

REGISTER_FORM(DinsLogicalForm, DcMenu::dins_logical);


#endif	//	__DINS_LOGICAL_FORM_H__

