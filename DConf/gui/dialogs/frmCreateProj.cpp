#include "frmCreateProj.h"

#include <QRegularExpressionValidator>

#include <dpc/gui/dialogs/msg_box/MsgBox.h>

#include "project/dc_project.h"

frmCreateProj::frmCreateProj(DcProject *project, QWidget *parent)
    : QDialog(parent)
    , m_project{project}
{
	ui.setupUi(this);
    this->setWindowTitle("Свойства проекта");
    ui.btnCreate->setText("Cохранить");

	QRegularExpression regex("^[-a-zA-Z0-9а-яА-Я_\\s]*$");
    ui.lineName->setValidator(new QRegularExpressionValidator(regex, this));

    connect(ui.bntCancel, &QPushButton::clicked, this, &frmCreateProj::reject);
    connect(ui.btnCreate, &QPushButton::clicked, this, &frmCreateProj::onCreateButton);

    ui.lineName->setText(project->name());
    ui.lineObject->setText(project->object());
    ui.lineAuthor->setText(project->author());
    ui.textDescription->setPlainText(project->desc());
}

void frmCreateProj::onCreateButton()
{
    auto name = ui.lineName->text().trimmed();
    if (name.isEmpty()) {
        Dpc::Gui::MsgBox::error("Имя проекта не может быть пустым");
        return;
    }

    m_project->setName(name);
    m_project->setAuthor(ui.lineAuthor->text());
    m_project->setObject(ui.lineObject->text());
    m_project->setDesc(ui.textDescription->toPlainText());
    m_project->save();
    accept();
}
