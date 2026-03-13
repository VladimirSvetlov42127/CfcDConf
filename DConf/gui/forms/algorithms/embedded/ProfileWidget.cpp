#include "ProfileWidget.h"

#include <QDebug>

#include "gui/forms/algorithms/embedded/DcAlgDefines.h"

namespace {
	const QString g_DefaultStyleSheet = 
		"QGroupBox {"
			"background-color: %1;"
			"border: 2px solid gray;"
			"border-radius: 5px;"
			"margin-top: 10ex;"
		"}"
		"QGroupBox::title {"
			"subcontrol-origin: margin;"
			"padding: 5 0px;"
		"}";


	const QString g_CurrentProfileStyleSheet = g_DefaultStyleSheet.arg(Algs::Colors::CurrentProfie.name());
	const QString g_DefaultProfileStyleSheet = g_DefaultStyleSheet.arg("palette(window)");
}

ProfileWidget::ProfileWidget(int profile, const QString &title, QWidget *parent) :
	QGroupBox(title, parent),
	m_profile(profile)
{
}

void ProfileWidget::onProfileChanged(const QString &value)
{
//    qDebug() << "ProfileChnaged" << value;
	setStyleSheet(value.toInt() == m_profile ? g_CurrentProfileStyleSheet : g_DefaultProfileStyleSheet);
}
