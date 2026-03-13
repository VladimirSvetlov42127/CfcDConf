#ifndef DCDBMANAGER_H
#define DCDBMANAGER_H

#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <map>

#include <qstring.h>
#include <QObject>

#include <db/sqlite/sqlite3.h>
#include <data_model/dc_controller.h>

#define gDbManager DcDbManager::getInstance()

class DcDbManager : public QObject
{
    Q_OBJECT
public:
    static DcDbManager& getInstance();

    DcController::UPtr load(const QString &filePath, const QString &name = QString());
    bool execute(int32_t uid, const QString &transaction);
    bool beginTransaction(int32_t uid);
    bool endTransaction(int32_t uid);
    void close(int32_t uid);
    void closeAll();

    bool isActive() const;
    bool testEvent();
    bool stepTransaction();

signals:
    void error(const QString &msg, const QString &filePath, DcController *device);

private:
    struct StorageItem;

    DcDbManager();
    ~DcDbManager();
    DcDbManager(DcDbManager const&) = delete;
    DcDbManager& operator= (DcDbManager const&) = delete;
    DcDbManager(DcDbManager&&) = delete;
    DcDbManager& operator= (DcDbManager&&) = delete;

    bool exec(StorageItem* item, const char *sql, int(*callback)(void*, int, char**, char**), void *data);
    StorageItem* getStorage(int32_t uid) const;
    void makeError(const QString &errMsg, StorageItem* item);
    int32_t freeUid() const;

private:
	std::queue<std::pair<int32_t, QString>> m_transactions;
    std::map<int32_t, std::unique_ptr<StorageItem>> m_connections;

	std::thread m_workerThread;
	std::mutex m_queueMutex;
    std::condition_variable m_queueCheck;

    bool m_isActive;
};

#endif // DCDBMANAGER_H
