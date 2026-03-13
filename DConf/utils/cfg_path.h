#pragma once

#include <qstring.h>
#include <qfileinfo.h>

namespace cfg
{
    struct path
	{
        // Debug - Абсолютный путь к рабочему каталогу приложения
        // Release - Абсолютный путь к папке с исполняемым файлом приложения
        static QString appDir();

        // На уровень выше appDir() т.е.:
        // Debug - Зависит от параметров(рабочий каталог) запуска отладки
        // Release - Абсолютный путь к корню куда установлен DTools
        static QString appRootDir();

        //File systrm operations
		static bool copy(const QString& src, const QString& dst);
	};
}
