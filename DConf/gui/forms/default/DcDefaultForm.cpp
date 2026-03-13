#include "DcDefaultForm.h"
#include <QLabel>
#include <QGridLayout>

namespace {
}

DcDefaultForm::DcDefaultForm(DcController *controller)
    : DcForm(controller, "Справочная информация")
{
	QGridLayout* layout = new QGridLayout(centralWidget());
    QLabel* label = new QLabel;
    label->setPixmap(QIcon(":/icons/dep_logo_gr.svg").pixmap(256, 256));
    layout->addWidget(label, 0, 0, Qt::AlignCenter);
}
