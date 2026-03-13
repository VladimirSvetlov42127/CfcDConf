#pragma once

#include <QDialog>
#include <QStandardItemModel>

#include "ui_frmOpenProj.h"
#include "project/dc_projects_manager.h"

class frmOpenProj : public QDialog
{
	Q_OBJECT

public:
    frmOpenProj(DcProjectsManager *projectsManager, QWidget *parent = nullptr);

    DcProject* project() const;

private slots:
    void projOpen();
    void projRemove();
    void projOpen(const QModelIndex&);

private:
    Ui::frmOpenProj ui;
    QStandardItemModel* m_model;
    DcProjectsManager* m_projectsManager;
    DcProject* m_project;
};
