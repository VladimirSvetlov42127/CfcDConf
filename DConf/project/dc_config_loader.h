#ifndef CONFIGLOADER_H
#define CONFIGLOADER_H

#include "data_model/dc_controller.h"

// ConfigLoader - Загрузчик кофигураций устройств из файла.
// Инкапсулирует в себе логику обновления конфигурации по шаблонам/версиям.
// А также обновлении значений параметров на основании таблиц привязок (matrix_alg, matrix_alg_cfc, matrix_signals).

// TO DO: Механизм уведомления об ошибках
// TO DO: Большой потенциал для оптимизации, возможно вынести часть работы в загрузку DBManager.

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

    void outputsToParams(DcController *config);
    void algsToParams(DcController *config);
    void cfcAlgsToParams(DcController *config);

private:
    std::map<QString, DcController::UPtr> m_cache;
};

#endif // CONFIGLOADER_H
