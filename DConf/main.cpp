//===================================================================================================================================================
//	Проверка утечек памяти
//===================================================================================================================================================
#ifdef _DEBUG
	//#define _MEMORY_LEAK                                      //  Раскомментировать при проверке утечек памяти
	#ifdef _MEMORY_LEAK
		#include <vld.h>
	#endif
#endif

#include "DConf.h"

#include <QtWidgets/QApplication>
#include <qtranslator.h>
#include <QLockFile>
#include <QDir>
#include <QMessageBox>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	a.setStyle("fusion");

	QTranslator tr;
	if (tr.load(QLocale(), "qt", "_", "./translations")) {
		a.installTranslator(&tr);
	}

#if defined(QT_DEBUG) || defined(TEST_BUILD)
#else
	QLockFile lockFile(QDir::temp().absoluteFilePath("LockFile.lock"));
	/*
	 * Пытаемся закрыть Lock File, если попытка безуспешна в течение 100 миллисекунд,
	 * значит уже существует Lock File созданный другим процессом.
	 * Следовательно, выбрасываем предупреждение и закрываем программу
	 */

	if (!lockFile.tryLock(100)) {
		QMessageBox msgBox;
		msgBox.setWindowIcon(QIcon(":/icons/info.svg"));
		msgBox.setIcon(QMessageBox::Warning);
		msgBox.setText("Приложение уже запущено.");
		msgBox.exec();
		return 1;
	}
#endif	

    DConf w;
    w.show();
    return a.exec();

//#ifdef _MEMORY_LEAK
//	_CrtDumpMemoryLeaks();
//#endif

	return 1;
}
