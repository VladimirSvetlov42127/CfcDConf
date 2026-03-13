#pragma once

#include <QDialog>

#include "ui_frmCreateProj.h"

class DcProject;

class frmCreateProj : public QDialog
{
	Q_OBJECT

public:
    frmCreateProj(DcProject* project, QWidget *parent = nullptr);

private slots:
    void onCreateButton();

private:
    Ui::frmCreateProj ui;
    DcProject* m_project;
};
