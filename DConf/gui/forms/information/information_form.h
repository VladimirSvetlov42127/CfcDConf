#ifndef INFORMATION_FORM_H
#define INFORMATION_FORM_H

#include <QLineEdit>

#include <gui/forms/DcFormFactory.h>

class InformationForm : public DcForm
{
public:
    InformationForm(DcController *controller);

    static bool isAvailableFor(DcController *controller);
    static void fillReport(DcIConfigReport *report);

private:
    QWidget *createTemplateInformationTab();
    QWidget *createDeviceAssigmentTab();

    QString settingsFile(DcController *device);
    QJsonObject loadPurposeSettings(DcController *device);
    void savePurposeFile();
    void loadPurposeFile();

    QLineEdit* m_companyEdit;
    QLineEdit* m_objectEdit;
    QLineEdit* m_placeEdit;
    QLineEdit* m_connectionEdit;
    QLineEdit* m_cabinetEdit;
    QLineEdit* m_implementerEdit;
    QLineEdit* m_dateEdit;
    QLineEdit* m_periodEdit;
    QLineEdit* m_reasonEdit;    
};

REGISTER_FORM(InformationForm, DcMenu::root);

#endif // INFORMATION_FORM_H
