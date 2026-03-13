#ifndef CFCALGSERVICE_H
#define CFCALGSERVICE_H

#include "service_manager/services/service.h"
#include "service_manager/services/alg_cfc/cfc_service_input.h"
#include "service_manager/services/alg_cfc/cfc_service_output.h"

#include "gui/forms/algorithms/custom/dep_parser.h"

class CfcAlgManager;

class CfcAlgService : public Service
{
    friend class CfcAlgManager;
public:
    using UPtr = std::unique_ptr<CfcAlgService>;

    virtual ~CfcAlgService();

    Service::Type type() const override;

    // Ид алгоритма гибкой логики для табилц привязки. Для обратной совместимости
    uint8_t id() const;

    // Включение/отключение алогоритма. Не скомпилированный алгоритм не может быть включен.
    bool isEnabled() const;
    void setEnabled(bool enabled);

    // Компиляция алгоритма
    bool isCompiled() const;
    void setCompiledData(const QByteArray &data);
    QByteArray compiledData() const;

    // Описание алгоритма
    QString description() const;
    void setDescription(const QString &desc);

    // Создает и добавляет вход/выход на свободную ножку pin.
    // Если нет свободных pin добавления не происходит и возвращает nullptr.
    // Запись в таблицу привязки для совместимости
    CfcServiceInput* makeInput();
    CfcServiceOutput* makeOutput();

    // Удаление входа/выхода из сервиса. !!!ПОСЛЕ ВЫЗОВА ЭТОЙ ФУНКЦИИ, ВСЕ УКАЗАТЕЛИ НА ВХОД/ВЫХОД СТАНУТ НЕ ВАЛИДНЫМИ!!!
    // Запись в таблицу привязки для совместимости
    void removeInput(CfcServiceInput* input);
    void removeOutput(CfcServiceOutput* output);

    // Парсер
    DepCfcParser* parser() const;

    // Сохранить данные алгоритма в файл
    bool save();

    bool hasInvalidInput() const;

private:
    // Приватный конструктор!!!
    CfcAlgService(uint8_t id, CfcAlgManager* manager);

    // Параметр привязки соответсвующий pin, или nullptr если такого нет.
    ParameterElement* bindElement(uint8_t pin) const;

    // Параметр для указания пути к скомпилированному файлу алгоритма.
    ParameterElement* fileElement() const;
    void setFileElement(ParameterElement* fileElement);

    // Возвращает свободную ножку(pin) алгоритма.
    std::optional<uint8_t> freePin() const;

    // Свободна ли ножка (pin).
    bool isFree(uint8_t pin) const;

    // Создание алгоритма гибкой логики в позиции position.
    // Возвращает nullptr, если в конфигурации нет параметров SP_FILE_FLEXLGFILES и SP_FLEXLGCROSSTABLE или
    // в них нет профилей соответсвующих позиции position.
    static CfcAlgService::UPtr create(uint8_t position, CfcAlgManager* manager);

    // Тоже самое что create(uint8_t position, CfcAlgManager* manager), но дополнительно идет загрузка
    // данных из файла гибкой логики соответсвующей позиции position.
    // Возвращает nullptr, при ошибках в обработке данных файла.
    static CfcAlgService::UPtr load(uint8_t position, CfcAlgManager* manager);

    // Создает и добавляет вход/выход на ножку pin. Если pin уже занят добавления не происходит и возвращает nullptr.
    // Запись в таблицу привязки для совместимости
    CfcServiceInput* makeInput(uint16_t ioID, uint8_t pin);
    CfcServiceOutput* makeOutput(uint16_t ioID, uint8_t pin);

    // Методы для работы с БД
    void dbInsert(ServiceIO *io);
    void dbRemove(ServiceIO *io);
    void dbUpdateProperties();

    // Пути к файлам.
    QString localGraphFileName() const;
    QString localImageFileName() const;
    QString localBcaFileName() const;
    QString deviceBcaFileName() const;

    // Очистка. Удаление всех входов/выходов.
    void clear();

    QString properties() const;

private:
    CfcAlgManager* m_manager;
    uint8_t m_id;
    std::vector<std::pair<ParameterElement*, ServiceIO*>> m_ios;

    DepCfcParser* m_parser;
    ParameterElement* m_fileElement = nullptr;
    QByteArray m_compiledData;
    QString m_descriptrion;
};

#endif // CFCALGSERVICE_H
