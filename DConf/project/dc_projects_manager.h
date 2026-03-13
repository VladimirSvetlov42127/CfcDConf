#ifndef DC_PROJECTS_MANAGER_H
#define DC_PROJECTS_MANAGER_H

#include <memory>
#include <vector>

#include <QObject>

#include "project/dc_project.h"

// DcProjectsManager - Менджер всех пользовательских проектов.
// Загрузка списка, создание/удаление.
// Используется папка пользовательских проектов по умолчанию. зависит от ОС.

class DcProjectsManager : public QObject
{
    Q_OBJECT

public:
    DcProjectsManager();

    // Путь к папке с проектами
    static QString path();

    // Список всех пользовательских проектов
    const DcProject::UPtrVector& projects() const;

    // Создать пользовательский проект
    DcProject* make(const QString &name, const QString &author, const QString &object, const QString &desc);
    DcProject* make();

    // Удалить пользовательский проект
    bool remove(DcProject *project);

signals:
    void error(const QString &);
    void aboutToRemove(DcProject *project);

public slots:
    bool load();

private:
    int nextProjNum() const;

private:
    DcProject::UPtrVector m_projects;
};

#endif // DC_PROJECTS_MANAGER_H
