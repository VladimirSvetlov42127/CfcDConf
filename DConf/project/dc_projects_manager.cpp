#include "dc_projects_manager.h"

#include <QDir>
#include <QStandardPaths>

namespace {

constexpr const char* g_Proj_prefix = "proj_";

bool isProjectDir(const QFileInfo &entry)
{
    if (!entry.isDir())
        return false;
    if (entry.baseName().size() < 6)
        return false;
    if (!entry.baseName().startsWith(g_Proj_prefix))
        return false;

    auto path = entry.absoluteFilePath();
    if (!QFile::exists(DcProject::infoFilePath(path)))
        return false;

    if (!QFile::exists(DcProject::topologyFilePath(path)))
        return false;

    return true;
}

} // namespace

DcProjectsManager::DcProjectsManager()
{
}

QString DcProjectsManager::path()
{
    static QString p = QString("%1/proj").arg(QStandardPaths::standardLocations(QStandardPaths::AppConfigLocation)
    #ifdef WIN32
        //WIN: C:/ProgramData/<Company>/<AppName>
        //LINUX: /etc/xdg/<Company>/<AppName>
        //MAC: /Users/<UserName>/Library/Application Support/<AppName>
        .at(1));
    #else
        //WIN: C:/Users/User/AppData/Local/<Company>/<AppName>
        //LINUX: /home/user/.config/<Company>/<AppName>
        //MAC: /Users/<UserName>/Library/Preferences/com.<Company>.<AppName>.plist
        .at(0));
    #endif

    return p;
}

const DcProject::UPtrVector &DcProjectsManager::projects() const
{
    return m_projects;
}

DcProject *DcProjectsManager::make(const QString &name, const QString &author, const QString &object, const QString &desc)
{
    auto projNum = nextProjNum();
    QString projectPath = QString("%1/%2%3").arg(path(), g_Proj_prefix).arg(projNum, 3, 10, QLatin1Char('0'));
    if (!QDir().mkpath(projectPath)) {
        emit error(QString("Не удалось создать папку %1").arg(projectPath));
        return nullptr;
    }

    auto project = std::make_unique<DcProject>(projectPath, name);
    if (name.trimmed().isEmpty())
        project->setName(QString("Проект %1").arg(projNum));
    project->setAuthor(author);
    project->setObject(object);
    project->setDesc(desc);
    project->setVersion(1);

    return m_projects.emplace_back(std::move(project)).get();
}

DcProject *DcProjectsManager::make()
{
    return make(QString(), QString(), QString(), QString());
}

bool DcProjectsManager::remove(DcProject *project)
{
    if (!project)
        return false;

    emit aboutToRemove(project);
    if (!QDir(project->path()).removeRecursively()) {
        emit error(QString("Не удалось удалить папку проекта %1").arg(project->path()));
        return false;
    }

    auto removedIt = std::remove_if(m_projects.begin(), m_projects.end(), [=](const DcProject::UPtr &p) {
        return p.get() == project;
    });
    m_projects.erase(removedIt, m_projects.end());
    return true;
}

bool DcProjectsManager::load()
{
    m_projects.clear();
    QDir projectsDir(path());
    if (!projectsDir.exists()) {
        emit error(QString("Не найдена директория проектов %1").arg(path()));
        return false;
    }

    auto list = projectsDir.entryInfoList(QDir::AllDirs | QDir::NoDotAndDotDot);
    m_projects.reserve(list.size());
    for (int i = 0; i < list.size(); ++i) {
        auto entry = list.at(i);
        if (!isProjectDir(entry))
            continue;

        auto project = std::make_unique<DcProject>(entry.absoluteFilePath());
        if (!project->load())
            continue;

        m_projects.emplace_back(std::move(project));
    }

    return true;
}

int DcProjectsManager::nextProjNum() const
{
    int maxNum = 0;
    QDir dir(path());
    if (dir.exists()) {
        for(auto &&entry: dir.entryInfoList()) {
            if (!isProjectDir(entry))
                continue;

            auto prefixSize = QString(g_Proj_prefix).size();
            int projNum = entry.fileName().mid(prefixSize).toInt();
            maxNum = std::max(maxNum, projNum);
        }
    }

    return ++maxNum;
}
