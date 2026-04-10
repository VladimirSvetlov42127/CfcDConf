#ifndef __DINS_EXTERNAL_FORM_H__
#define __DINS_EXTERNAL_FORM_H__

#include <gui/forms/DcFormFactory.h>

class DinsExternalForm : public DcForm
{
	Q_OBJECT

public:
	DinsExternalForm(DcController* controller);
	static bool isAvailableFor(DcController* controller);
    static void fillReport(DcIConfigReport* report);
};

REGISTER_FORM(DinsExternalForm, DcMenu::dins_external);

#endif	//	__DINS_EXTERNAL_FORM_H__

