#include "information_form.h"

#include <QTabWidget>
#include <QFormLayout>
#include <QJsonDocument>
#include <QJsonObject>
#include <QFileInfo>

#include "gui/editors/EditorsManager.h"

namespace TabName
{
const char *COMMON = "Общие";
const char *DEVICE_ASSIGMENT = "Назначение устройства";
} //TabName

namespace Settings
{
const char *Company = "company";
const char *Object = "object";
const char *Place = "place";
const char *Connection = "connection";
const char *Cabinet = "cabinet";
const char *Implementer = "implementer";
const char *Date = "date";
const char *Period = "period";
const char *Reason = "reason";
} //Settings

namespace Text
{
const char *NAME = "Название устройства";
const char *HDWPARTNUMBER = "Код устройства";
const char *SOFTWARE_ID = "Код ПО";
const char *SOFTWARE_VERSION = "Версия ПО";
const char *SOFTWARE_SUBVERSION = "Подверсия ПО";
const char *CFG_VERSION = "Версия конфигурации";

const char *Company = "Предприятие";
const char *Object = "Объект";
const char *Place = "Место установки";
const char *Connection = "Присоединение";
const char *Cabinet = "Устройство/шкаф";
const char *Implementer = "Исполнитель";
const char *Date = "Дата выдачи";
const char *Period = "Срок реализации";
const char *Reason = "Причина выдачи";
} //Text

InformationForm::InformationForm(DcController *controller)
    : DcForm(controller,  "Информация о конфигурации", false)
    , m_companyEdit(new QLineEdit)
    , m_objectEdit(new QLineEdit)
    , m_placeEdit(new QLineEdit)
    , m_connectionEdit(new QLineEdit)
    , m_cabinetEdit(new QLineEdit)
    , m_implementerEdit(new QLineEdit)
    , m_dateEdit(new QLineEdit)
    , m_periodEdit(new QLineEdit)
    , m_reasonEdit(new QLineEdit)
{
    QVBoxLayout *formLayout = new QVBoxLayout(centralWidget());

    QTabWidget *tabWidget = new QTabWidget;

    connect(m_companyEdit, &QLineEdit::editingFinished, this, &InformationForm::savePurposeFile);
    connect(m_objectEdit, &QLineEdit::editingFinished, this, &InformationForm::savePurposeFile);
    connect(m_placeEdit, &QLineEdit::editingFinished, this, &InformationForm::savePurposeFile);
    connect(m_connectionEdit, &QLineEdit::editingFinished, this, &InformationForm::savePurposeFile);
    connect(m_cabinetEdit, &QLineEdit::editingFinished, this, &InformationForm::savePurposeFile);
    connect(m_implementerEdit, &QLineEdit::editingFinished, this, &InformationForm::savePurposeFile);
    connect(m_dateEdit, &QLineEdit::editingFinished, this, &InformationForm::savePurposeFile);
    connect(m_periodEdit, &QLineEdit::editingFinished, this, &InformationForm::savePurposeFile);
    connect(m_reasonEdit, &QLineEdit::editingFinished, this, &InformationForm::savePurposeFile);
    loadPurposeFile();

    tabWidget->addTab(createTemplateInformationTab(), TabName::COMMON);
    tabWidget->addTab(createDeviceAssigmentTab(), TabName::DEVICE_ASSIGMENT);

    formLayout->addWidget(tabWidget);

}

bool InformationForm::isAvailableFor(DcController * controller)
{
    return true;
}

void InformationForm::fillReport(DcIConfigReport * report)
{
}

QWidget *InformationForm::createTemplateInformationTab()
{
    QWidget* tab = new QWidget;
    QGridLayout *tabLayout = new QGridLayout(tab);
    EditorsManager mg(controller(), tabLayout);

    auto makeReadOnly = [](ParamEditor *editor) { if (editor) editor->setFixedSize(400, 25); editor->setReadOnly(true); };
    makeReadOnly(mg.addLineEditor(SP_DEVICE_NAME, 0, Text::NAME));
    makeReadOnly(mg.addLineEditor(SP_HDWPARTNUMBER, 0, Text::HDWPARTNUMBER));
    makeReadOnly(mg.addLineEditor(SP_SOFTWARE_ID, 0, Text::SOFTWARE_ID));
    makeReadOnly(mg.addLineEditor(SP_SOFTWARE_VERTION, 1, Text::SOFTWARE_VERSION));
    makeReadOnly(mg.addLineEditor(SP_SOFTWARE_VERTION, 0, Text::SOFTWARE_SUBVERSION));
    makeReadOnly(mg.addLineEditor(SP_CFGVER, 0, Text::CFG_VERSION));

    mg.addStretch();

    return tab;
}

QWidget *InformationForm::createDeviceAssigmentTab()
{
    auto tab = new QWidget;
    QFormLayout *purposeLayout = new QFormLayout(tab);

    m_companyEdit->setFixedSize(400,25);
    purposeLayout->setSizeConstraint(QLayout::SetFixedSize);

    purposeLayout->addRow(Text::Company, m_companyEdit);
    purposeLayout->addRow(Text::Object, m_objectEdit);
    purposeLayout->addRow(Text::Place, m_placeEdit);
    purposeLayout->addRow(Text::Connection, m_connectionEdit);
    purposeLayout->addRow(Text::Cabinet, m_cabinetEdit);
    purposeLayout->addRow(Text::Implementer, m_implementerEdit);
    purposeLayout->addRow(Text::Date, m_dateEdit);
    purposeLayout->addRow(Text::Period, m_periodEdit);
    purposeLayout->addRow(Text::Reason, m_reasonEdit);

    return tab;
}

void InformationForm::loadPurposeFile()
{
    auto obj = loadPurposeSettings(controller());
    m_companyEdit->setText(obj.value(Settings::Company).toString());
    m_objectEdit->setText(obj.value(Settings::Object).toString());
    m_placeEdit->setText(obj.value(Settings::Place).toString());
    m_connectionEdit->setText(obj.value(Settings::Connection).toString());
    m_cabinetEdit->setText(obj.value(Settings::Cabinet).toString());
    m_implementerEdit->setText(obj.value(Settings::Implementer).toString());
    m_dateEdit->setText(obj.value(Settings::Date).toString());
    m_periodEdit->setText(obj.value(Settings::Period).toString());
    m_reasonEdit->setText(obj.value(Settings::Reason).toString());
}

void InformationForm::savePurposeFile()
{
    QFile file(settingsFile(controller()));
    if (!file.open(QIODevice::WriteOnly)) return;

    QJsonObject obj;
    obj[Settings::Company] = m_companyEdit->text();
    obj[Settings::Object] = m_objectEdit->text();
    obj[Settings::Place] = m_placeEdit->text();
    obj[Settings::Connection] = m_connectionEdit->text();
    obj[Settings::Cabinet] = m_cabinetEdit->text();
    obj[Settings::Implementer] = m_implementerEdit->text();
    obj[Settings::Date] = m_dateEdit->text();
    obj[Settings::Period] = m_periodEdit->text();
    obj[Settings::Reason] = m_reasonEdit->text();
    file.write(QJsonDocument(obj).toJson());
}

QString InformationForm::settingsFile(DcController *device)
{
    return QString("%1/purpose.json").arg(QFileInfo(device->path()).absolutePath());
}

QJsonObject InformationForm::loadPurposeSettings(DcController *device)
{
    QJsonObject obj;
    QFile file(settingsFile(device));
    if (file.open(QIODevice::ReadOnly)) {
        auto jsonDoc = QJsonDocument::fromJson(file.readAll());
        if (!jsonDoc.isNull()) obj = jsonDoc.object();
    }

    return obj;
}

