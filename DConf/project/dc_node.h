#ifndef DCNODE_H
#define DCNODE_H

#include <memory>
#include <vector>

#include <QString>
#include <QIcon>
#include <QObject>

// DcNode - Базовый класс узла дерева в иерархической структуре дерева проектов.

class DcNode : public QObject
{
    Q_OBJECT

public:
    // Типы узлов
    enum Type {
        NoType = 0,     // Не установлен
        ProjectType,    // Проект
        DirType,        // Папка
        DeviceType      // Конфигурация устройства
    };

    using UPtr = std::unique_ptr<DcNode>;
    using UPtrVector = std::vector<UPtr>;

    DcNode(int32_t id, const QString &name, DcNode* parent = nullptr);
    virtual ~DcNode();

    bool isValid() const;

    // id Узла.
    int32_t id() const { return m_id; }

    // Тип узла.
    virtual Type type() const { return NoType; }

    // Абсолютный путь к папке узла в файловой системе.
    // Актуально для узлов типа "Проект" и "Конфигурация устройства".
    // Возвращает пустую строку, если у узла нет соотвествующей папки.
    virtual QString path() const { return QString(); };

    // Иконка узла.
    virtual QIcon icon() const;

    // Имя узла.
    QString name() const { return m_name; }
    void setName(const QString &name);

    // Имя узла включая все родительские узлы, через разделитель.
    QString fullText() const;

    // Родительский узел.
    DcNode* parent();
    const DcNode* parent() const;

    // Позиция узла в списке потомков родителя.
    // Если нет родительского узла, возвращает 0.
    size_t position() const;

    // Количество потомков узла.
    size_t childsSize() const;

    // Возвращает потомка узла под позицией pos, или nullptr если неверная позиция.
    DcNode* child(size_t pos);
    const DcNode* child(size_t pos) const;

    // Перемещает и вставляет childs в список потомков узла на позицию pos.
    // Забирает право владения.
    bool insertChilds(size_t pos, DcNode::UPtrVector childs);

    // Удаляет из списка потомков узла и возвращает count потомков с позиции pos.
    // Отдает право владения.
    DcNode::UPtrVector takeChilds(size_t pos, size_t count);

    // Удаление потомка на позиции pos
    bool removeChild(size_t pos);

signals:
    void nameChanged(const QString &name);
    void fullTextChanged();

protected:
    void setId(int32_t id);
    int32_t maxId() const;

private:
    void setParent(DcNode *parent);

private:
    int32_t m_id = 0;
    DcNode* m_parent = nullptr;
    QString m_name;
    DcNode::UPtrVector m_childs;
};
#endif // DCPROJECTNODE_H
