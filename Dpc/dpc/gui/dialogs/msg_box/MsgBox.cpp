#include "MsgBox.h"

#include <qmessagebox.h>
#include <qapplication.h>
#include <qstyle.h>
#include <qdebug.h>
#include <QPushButton>

#include <dpc/gui/gui.h>

namespace {
    int execMsgBox(QMessageBox *box)
    {
		box->setWindowIcon(QApplication::windowIcon());
		return box->exec();
	}

    QPixmap loadPixmap(const QString &fileName)
    {
        QPixmap pixmap(fileName);
        return pixmap.scaled(QSize(32, 32), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }
} // namespace

namespace Dpc::Gui
{
	void MsgBox::info(const QString& msg)
	{
		QMessageBox msgBox(QMessageBox::Information, "Информация", msg, QMessageBox::Ok);
        msgBox.setIconPixmap(loadPixmap(":/icons/info.svg"));
		execMsgBox(&msgBox);
	}

	void MsgBox::warning(const QString& msg)
	{
		QMessageBox msgBox(QMessageBox::Warning, "Предупреждение", msg, QMessageBox::Ok);
        msgBox.setIconPixmap(loadPixmap(":/icons/warning.svg"));
		execMsgBox(&msgBox);
	}

	void MsgBox::error(const QString& msg)
	{
		QMessageBox msgBox(QMessageBox::Critical, "Ошибка", msg, QMessageBox::Ok);
        msgBox.setIconPixmap(loadPixmap(":/icons/error.svg"));
		execMsgBox(&msgBox);
	}

	bool MsgBox::question(const QString& msg)
	{
		QMessageBox msgBox(QMessageBox::Question, "Подтверждение", msg);
        msgBox.setIconPixmap(loadPixmap(":/icons/question.svg"));
        auto yesButton = msgBox.addButton("Да", QMessageBox::AcceptRole);
		auto noButton = msgBox.addButton("Нет", QMessageBox::RejectRole);
        msgBox.setDefaultButton(noButton);
        execMsgBox(&msgBox);

        return msgBox.clickedButton() == yesButton;
	}
} // namespace
