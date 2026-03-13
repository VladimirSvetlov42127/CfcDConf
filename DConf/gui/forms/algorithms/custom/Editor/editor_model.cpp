#include <gui/forms/algorithms/custom/Editor/editor_model.h>

//===================================================================================================================================================
//	Подключение стандартных библиотек
//===================================================================================================================================================

//===================================================================================================================================================
//	Подключение сторонних библиотек
//===================================================================================================================================================

//===================================================================================================================================================
//	Подключение библиотек QT
//===================================================================================================================================================
#include <QList>
#include <QMimeData>

//===================================================================================================================================================
//	Подключение модулей проекта
//===================================================================================================================================================

//===================================================================================================================================================
//	Списки данных проекта
//===================================================================================================================================================
#define SECTION_INPUT_OUTPUT    "Входы/Выходы"
#define SECTION_LOGICAL         "Логические"
#define SECTION_SELECTOR        "Выбор"
#define SECTION_TRIGGERS        "Триггеры"
#define SECTION_CONTROL         "Управление"
#define SECTION_GENERATORS      "Генераторы"

struct FlexEditorNode {
    QString index;
    QString name;
    QString icon;
    QString section; };

QList<FlexEditorNode> FlexEditorList = {
    { "And", "AND", ":/icons/function.svg", SECTION_LOGICAL },
    { "Or", "OR", ":/icons/function.svg", SECTION_LOGICAL },
    { "Not", "NOT", ":/icons/function.svg", SECTION_LOGICAL },
    { "Xor", "XOR", ":/icons/function.svg", SECTION_LOGICAL},

    { "BI", "Дискретный вход", ":/icons/bi.svg", SECTION_INPUT_OUTPUT },
    { "BO", "Дискретный выход", ":/icons/bo.svg", SECTION_INPUT_OUTPUT },

    { "Generator", "Генератор", ":/icons/generator.svg", SECTION_GENERATORS },
    { "ImpulsePF", "Импульс по ПФ", ":/icons/impulsePF.svg", SECTION_GENERATORS },
    { "ImpulseZF", "Импульс по ЗФ", ":/icons/impulseZF.svg", SECTION_GENERATORS },

    { "ReturnDelay", "Задержка возврата", ":/icons/returndelay.svg", SECTION_CONTROL },
    { "TriggerDelay", "Задержка срабатывания", ":/icons/triggerdelay.svg", SECTION_CONTROL},

    { "RsTrigger", "RS-триггер", ":/icons/rstrigger.svg", SECTION_TRIGGERS }
};


//===================================================================================================================================================
//	Конструктор и деструктор класса
//===================================================================================================================================================
EditorModel::EditorModel(QObject* parent) : QAbstractItemModel(parent)
{
    _root_item = new EditorItem(QString(), QIcon(), QString());

    for (int i = 0; i < FlexEditorList.count(); i++) {
        FlexEditorNode node = FlexEditorList.at(i);
        EditorItem* section_item = nullptr;
        for (int ii = 0; ii < _root_item->ChildCount(); ii++)
            if (_root_item->Child(ii)->Name() == node.section) {
                section_item = _root_item->Child(ii);
                break; }
        if (!section_item) {
            section_item = new EditorItem(node.section, QIcon(":/icons/extension.svg"), QString(), _root_item);
            _root_item->AppendChild(section_item); }
        section_item->AppendChild(new EditorItem(node.name, QIcon(node.icon), node.index, section_item)); }
}

EditorModel::~EditorModel()
{
    delete _root_item;
}


//===================================================================================================================================================
//	Перегружаемые методы класса
//===================================================================================================================================================
int EditorModel::columnCount(const QModelIndex& parent) const
{
    return 1;
}

int EditorModel::rowCount(const QModelIndex& parent) const
{
    EditorItem* parent_item;
    if (parent.column() > 0) return 0;

    !parent.isValid() ? parent_item = _root_item : parent_item = static_cast<EditorItem*>(parent.internalPointer());
    return parent_item->ChildCount();
}

Qt::ItemFlags EditorModel::flags(const QModelIndex& index) const
{
    if (!index.isValid()) return Qt::ItemFlags();
    EditorItem* item = static_cast<EditorItem*>(index.internalPointer());
    if (!item->Index().isEmpty()) return QAbstractItemModel::flags(index) | Qt::ItemIsDragEnabled;

    return QAbstractItemModel::flags(index);
}

QModelIndex EditorModel::index(int row, int column, const QModelIndex& parent) const
{
    if (!hasIndex(row, column, parent)) return QModelIndex();
    EditorItem* parent_item;
    parent.isValid() ? parent_item = static_cast<EditorItem*>(parent.internalPointer()) : parent_item = _root_item;
    EditorItem* child_item = parent_item->Child(row);

    return child_item ? createIndex(row, column, child_item) : QModelIndex();
}

QModelIndex EditorModel::parent(const QModelIndex& index) const
{
    if (!index.isValid()) return QModelIndex();

    EditorItem* child_item = static_cast<EditorItem*>(index.internalPointer());
    EditorItem* parent_item = child_item->ParentItem();

    if (parent_item == _root_item) return QModelIndex();
    return createIndex(parent_item->row(), 0, parent_item);
}

QVariant EditorModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid()) return QVariant();

    EditorItem* item = static_cast<EditorItem*>(index.internalPointer());
    if (role == Qt::DisplayRole) return item->Name();
    if (role == Qt::DecorationRole) return item->Icon();
    if (role == Qt::UserRole) return item->Index();

    return QVariant();
}


QMimeData* EditorModel::mimeData(const QModelIndexList& indexes) const
{
    QMimeData* mime = new QMimeData();
    QModelIndex index = indexes.value(0);

    if (index.isValid()) {
        EditorItem* item = static_cast<EditorItem*>(index.internalPointer());
        mime->setData("Node", item->Index().toUtf8()); }

    return mime;
}
