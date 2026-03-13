#include "cfc_editor_model.h"


//===================================================================================================================================================
//	Подключение библиотек QT
//===================================================================================================================================================
#include <QList>
#include <QMimeData>

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
CfcEditorModel::CfcEditorModel(QObject* parent) : QAbstractItemModel(parent)
{
    _root_item = new CfcEditorItem(QString(), QIcon(), QString());

    for (int i = 0; i < FlexEditorList.count(); i++) {
        FlexEditorNode node = FlexEditorList.at(i);
        CfcEditorItem* section_item = nullptr;
        for (int ii = 0; ii < _root_item->childCount(); ii++)
            if (_root_item->child(ii)->name() == node.section) {
                section_item = _root_item->child(ii);
                break;
            }
        if (!section_item) {
            section_item = new CfcEditorItem(node.section, QIcon(":/icons/extension.svg"), QString(), _root_item);
            _root_item->appendChild(section_item);
        }
        section_item->appendChild(new CfcEditorItem(node.name, QIcon(node.icon), node.index, section_item));
    }
}

CfcEditorModel::~CfcEditorModel()
{
    delete _root_item;
}


//===================================================================================================================================================
//	Перегружаемые методы класса
//===================================================================================================================================================
int CfcEditorModel::columnCount(const QModelIndex& parent) const
{
    return 1;
}

int CfcEditorModel::rowCount(const QModelIndex& parent) const
{
    CfcEditorItem* parent_item;
    if (parent.column() > 0) return 0;

    !parent.isValid() ? parent_item = _root_item : parent_item = static_cast<CfcEditorItem*>(parent.internalPointer());
    return parent_item->childCount();
}

Qt::ItemFlags CfcEditorModel::flags(const QModelIndex& index) const
{
    if (!index.isValid()) return Qt::ItemFlags();
    CfcEditorItem* item = static_cast<CfcEditorItem*>(index.internalPointer());
    if (!item->index().isEmpty()) return QAbstractItemModel::flags(index) | Qt::ItemIsDragEnabled;

    return QAbstractItemModel::flags(index);
}

QModelIndex CfcEditorModel::index(int row, int column, const QModelIndex& parent) const
{
    if (!hasIndex(row, column, parent)) return QModelIndex();
    CfcEditorItem* parent_item;
    parent.isValid() ? parent_item = static_cast<CfcEditorItem*>(parent.internalPointer()) : parent_item = _root_item;
    CfcEditorItem* child_item = parent_item->child(row);

    return child_item ? createIndex(row, column, child_item) : QModelIndex();
}

QModelIndex CfcEditorModel::parent(const QModelIndex& index) const
{
    if (!index.isValid()) return QModelIndex();

    CfcEditorItem* child_item = static_cast<CfcEditorItem*>(index.internalPointer());
    CfcEditorItem* parent_item = child_item->parentItem();

    if (parent_item == _root_item) return QModelIndex();
    return createIndex(parent_item->row(), 0, parent_item);
}

QVariant CfcEditorModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid()) return QVariant();

    CfcEditorItem* item = static_cast<CfcEditorItem*>(index.internalPointer());
    if (role == Qt::DisplayRole) return item->name();
    if (role == Qt::DecorationRole) return item->icon();
    if (role == Qt::UserRole) return item->index();

    return QVariant();
}


QMimeData* CfcEditorModel::mimeData(const QModelIndexList& indexes) const
{
    QMimeData* mime = new QMimeData();
    QModelIndex index = indexes.value(0);

    if (index.isValid()) {
        CfcEditorItem* item = static_cast<CfcEditorItem*>(index.internalPointer());
        mime->setData("Node", item->index().toUtf8()); }

    return mime;
}
