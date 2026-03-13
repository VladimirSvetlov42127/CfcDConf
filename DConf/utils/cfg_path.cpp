#include "cfg_path.h"

#include <qdir.h>
#include <qfiledialog.h>
#include <qcoreapplication.h>

namespace {
    const QString proj_prefix = "proj_";

    QDir g_appDir() {
#ifdef QT_DEBUG
        return QDir();
#else
        return QDir(QCoreApplication::applicationDirPath());
#endif
    }
}

QString cfg::path::appDir()
{
    return g_appDir().absolutePath();
}

QString cfg::path::appRootDir()
{
    auto dir = g_appDir();
    dir.cdUp();
    return dir.absolutePath();
}

bool cfg::path::copy(const QString& src, const QString& dst)
{
	QDir dir(src);
	if (!dir.exists())
		return false;

	const QString src_dir = QDir::toNativeSeparators(dir.absolutePath()) + QDir::separator();
	const QString dst_dir = QDir::toNativeSeparators(QDir(dst).absolutePath()) + QDir::separator();

	QDir projpath(QDir::currentPath());
    for(auto& d: dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
		QString dst_path = dst_dir + d;
		projpath.mkpath(dst_path);
		if (!copy(src_dir + d, dst_path))
			return false;
	}

    foreach(auto &f, dir.entryList(QDir::Files)) {
		QString dst_path = dst_dir + f;
		if (QFile().exists(dst_path))
			QFile().remove(dst_path);

		if (!QFile::copy(src_dir + f, dst_path))
			return false;
	}
	return true;
}
