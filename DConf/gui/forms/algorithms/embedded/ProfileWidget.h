#pragma once

#include <qgroupbox.h>

class ProfileWidget : public QGroupBox
{
	Q_OBJECT
public:
	ProfileWidget(int profile, const QString &title, QWidget *parent = nullptr);

public slots:
	void onProfileChanged(const QString &value);

private:
	int m_profile;
};