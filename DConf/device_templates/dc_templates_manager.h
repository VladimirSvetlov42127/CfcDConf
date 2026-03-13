#ifndef DCTEMPLATESMANAGER_H
#define DCTEMPLATESMANAGER_H

#include <unordered_set>
#include <vector>

#include <QObject>
#include <QStringList>
#include <QMap>

#define gTemplatesManager DcTemplatesManager::getInstance()

// DcTemplatesManager - класс для отобржаения информации о шаблонах конфигураций устройств из базы DTB.
// Загрузка информации происходит из папки по умолчанию.

class DcTemplatesManager : public QObject
{
    Q_OBJECT

public:
    DcTemplatesManager(const DcTemplatesManager&) = delete;
    DcTemplatesManager(DcTemplatesManager&&) = delete;
    DcTemplatesManager& operator=(const DcTemplatesManager&) = delete;
    DcTemplatesManager& operator=(DcTemplatesManager&&) = delete;

    static DcTemplatesManager& getInstance();

    // Путь к папке с шаблонами
    QString path() const;

    // сравнение на совместимость конфигураций с кодами ПО softCode1 и softCode2
    bool isCompatibleDevices(uint16_t softCode1, uint16_t softCode2) const;

    // Ключ - тип устройства, Значение - путь к файлу шаблона конфигурации
    const QMap<QString, QString>& deviceTemplates() const;

    // Ключ - модификация deprotec, Значение - путь к файлу шаблона конфигурации
    const QMap<QString, QString>& deprotecModifications() const;

    // Генерирует путь к файлу конфигурации устройства deviceType и модификации modification (если есть).
    // Такого файла может не существовать!!!
    QString deviceTemplateFilePath(const QString &deviceType, const QString &modification) const;

signals:
    void error(const QString &);

public slots:
    bool load();

private:
    using SoftCodeSet = std::unordered_set<uint16_t>;
    using SoftCodeSetContainer = std::vector<SoftCodeSet>;

    DcTemplatesManager();

    bool loadCompatibleSoftCodes();
    bool loadDeviceTemplates();
    bool loadDeprotecModifications();

private:
    SoftCodeSetContainer m_compatibleSoftCodes;
    QMap<QString, QString> m_deviceTemplates;
    QMap<QString, QString> m_deprotecModifications;
};

#endif // DCTEMPLATESMANAGER_H
