#ifndef __DINS_VIRTUAL_FORM_H__
#define __DINS_VIRTUAL_FORM_H__

#include <gui/forms/DcFormFactory.h>

class DinsVirtualForm : public DcForm
{
	Q_OBJECT
public:
	DinsVirtualForm(DcController* controller);
	static bool isAvailableFor(DcController* controller);
    static void fillReport(DcIConfigReport* report);
};

REGISTER_FORM(DinsVirtualForm, DcMenu::dins_virtual);


#endif	//	__DINS_VIRTUAL_FORM_H__

