#ifndef DCPROJECT_H
#define DCPROJECT_H

#include <QDateTime>

#include "project/dc_node.h"

// DcProject - Узел дерева в иерархической структуре дерева проектов соответствующий "Проекту".
// Отображает структуру проекта в иерархическом виде,
// несмотря на то что информация в файлах проекта, продолжает хранится в линейном виде, для обратной совместимости.
// При вызове метода load, загружается только информация о проекте и его иерархической структуре.
// При вызове метода save, сохраняется только информация о проекте и его иерархической структуре.

// TO DO: Механизм уведомления об ошибках

class DcProject : public DcNode
{
public:
    using UPtr = std::unique_ptr<DcProject>;
    using UPtrVector = std::vector<UPtr>;

    DcProject(const QString &path, const QString &name = QString());
    virtual ~DcProject();

    // Пути к файлам info и topology, с учётом префикса prefix.
    // Если prefix не указан - возвращается имя файла.
    static QString infoFilePath(const QString& prefix = QString());
    static QString topologyFilePath(const QString& prefix = QString());

    DcNode::Type type() const override { return DcNode::ProjectType; }

    QIcon icon() const override;

    // Абсолютный путь к папке проекта в файловой системе.
    QString path() const override { return m_path; }

    // Объект проекта
    QString object() const { return m_object; }
    void setObject(const QString &obj) { m_object = obj; }

    // Автор проекта
    QString author() const { return m_author; }
    void setAuthor(const QString &author) { m_author = author; }

    // Описание проекта
    QString desc() const { return m_desc; }
    void setDesc(const QString &desc) { m_desc = desc; }

    // Время создания и изменения проекта
    QDateTime createdTime() const { return m_createdTime; }
    QDateTime updatedTime() const { return m_updatedTime; }

    // Версия проекта
    uint32_t version() const { return m_version; }
    void setVersion(uint32_t version) { m_version = version; }

    // Загрузка и сохранение информации и структуры проекта в/из файловую систему.
    bool load();
    bool save() const;

    // Создание в проекте узла типа type.
    // Если тип узла - конфигурация устройства, создается папка для узла в файловой системе.
    DcNode* createNode(const QString &name, DcNode::Type type, DcNode* parent);

    // Удаление данных узла node из проекта.
    // Если тип узла - конфигурация устройства, удаляется папка узла из файловой системы.
    bool removeNodeData(DcNode *node);

private:
    QString m_path;
    QString m_object;
    QString m_author;
    QString m_desc;
    QDateTime m_createdTime;
    mutable QDateTime m_updatedTime;
    uint16_t m_version;
};

#endif // DCPROJECT_H
