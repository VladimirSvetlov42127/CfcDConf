#ifndef CONFIGLOADER_H
#define CONFIGLOADER_H

#include "data_model/dc_controller.h"

// ConfigLoader - Загрузчик кофигураций устройств из файла.
// Инкапсулирует в себе логику обновление конфигурации по шаблонам или версиям.

// TO DO: Механизм уведомления об ошибках

class ConfigLoader : public QObject
{
    Q_OBJECT
public:
    ConfigLoader(QObject *parent = nullptr);
    ~ConfigLoader();

    DcController::UPtr load(const QString &filePath);
    DcController::UPtr load(const QString &filePath, const QString &name, bool updateFromTemplate);

    void freeCache();

signals:
    void error(const QString &errorMsg);

private slots:
    void onDbError(const QString &msg, const QString &filePath, DcController *device);

private:
    void updateController(DcController *controller);
    void updateParams(DcController *contr, DcController *temp);
    void updateBoards(DcController *contr, DcController *temp);
    void updateSignals(DcController *contr, DcController *temp);
    void updateAlgs(DcController *contr, DcController *temp);
    void updateSettings(DcController *contr, DcController *temp);

private:
    std::map<QString, DcController::UPtr> m_cache;
};

#endif // CONFIGLOADER_H
