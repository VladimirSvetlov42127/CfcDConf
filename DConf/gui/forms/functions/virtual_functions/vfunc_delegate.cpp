#include "vfunc_delegate.h"


//===================================================================================================================================================
//	Подключение модулей проекта
//===================================================================================================================================================
#include <dpc/gui/delegates/SpinBoxDelegate.h>
#include "gui/editors/EditorsManager.h"
#include "service_manager/services/func/func_service.h"

//===================================================================================================================================================
//	Списки данных класса
//===================================================================================================================================================
namespace {
    const QStringList on_off = { "Откл", "Вкл" };
    const QStringList groups = { "Группа уставок 1", "Группа уставок 2", "Группа уставок 3", "Группа уставок 4"};
}


//===================================================================================================================================================
//	Конструктор класса
//===================================================================================================================================================
VFuncDelegate::VFuncDelegate(DcController* dc_controller, QObject* parent) : QStyledItemDelegate(parent)
{
    _controller = dc_controller;
    int count = _controller->serviceManager()->funcService().funcList().size();
    for (int i = 0; i < count; i++) {
        auto func_type = _controller->serviceManager()->funcService().funcList().at(i).get()->type();
        uint16_t value = _controller->serviceManager()->funcService().funcList().at(i).get()->argValue();
        InnerDelegate inner;
        inner.type = func_type;
        inner.delegate = getDelegate(func_type, value);
        _delegates.append(inner);
    }
}


//===================================================================================================================================================
//	Перегружаемые методы класса
//===================================================================================================================================================
QWidget* VFuncDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    if (!index.isValid())
        return nullptr;

    uint8_t type = index.data(Qt::UserRole).toUInt();
    uint16_t value = index.data(Qt::DisplayRole).toInt();
    auto func_type = static_cast<VFunc::Type>(type);
    int row = index.row();
    setDelegate(row, func_type, value);
    InnerDelegate inner = _delegates.at(row);
    if (inner.delegate) return inner.delegate->createEditor(parent, option, index);

    return QStyledItemDelegate::createEditor(parent, option, index);
}

void VFuncDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
    if (!index.isValid())
        return;

    InnerDelegate inner = _delegates.at(index.row());
    inner.delegate ? inner.delegate->setEditorData(editor, index) : QStyledItemDelegate::setEditorData(editor, index);
}

void VFuncDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
    if (!index.isValid())
        return;

    InnerDelegate inner = _delegates.at(index.row());
    inner.delegate ? inner.delegate->setModelData(editor, model, index) : QStyledItemDelegate::setModelData(editor, model, index);
}

void VFuncDelegate::updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    if (!index.isValid())
        return;

    InnerDelegate inner = _delegates.at(index.row());
    inner.delegate ? inner.delegate->updateEditorGeometry(editor, option, index) : QStyledItemDelegate::updateEditorGeometry(editor, option, index);
}

void VFuncDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    if (!index.isValid())
        return;

    uint8_t type = index.data(Qt::UserRole).toUInt();
    uint16_t value = index.data(Qt::DisplayRole).toInt();
    auto func_type = static_cast<VFunc::Type>(type);
    int row = index.row();
    setDelegate(row, func_type, value);
    InnerDelegate inner = _delegates.at(index.row());
    inner.delegate ? inner.delegate->paint(painter, option, index) : QStyledItemDelegate::paint(painter, option, index);
}


//===================================================================================================================================================
//	Вспомогательные методы класса
//===================================================================================================================================================
void VFuncDelegate::setDelegate(int index, VFunc::Type type, uint16_t value) const
{
    if (_delegates.at(index).type == type)
        return;

    InnerDelegate inner = _delegates.at(index);
    delete inner.delegate;
    inner.type = type;
    inner.delegate = getDelegate(type, value);
    _delegates[index] = inner;
    return;

    return;
}

QStringList VFuncDelegate::xcbrList() const
{
    QStringList xcbrList;
    auto xcbr = controller()->paramsRegistry().parameter(SP_XCBRCFG);
    if (xcbr)
        for (size_t i = 0; i < xcbr->profilesCount(); i++)
            xcbrList << QString("Выключатель %1").arg(i + 1);

    return xcbrList;
}

QList<ComboBoxDelegate::Item> VFuncDelegate::dinList(uint16_t value) const
{
    QList<ComboBoxDelegate::Item> result;
    QList<VirtualInputSignal*> din_list =controller()->serviceManager()->vdins();
    for (int i = 0; i < din_list.count();i++) {
        if (din_list.at(i)->source()) {
            if (din_list.at(i)->subTypeID() != value)
                continue;
        }
        ComboBoxDelegate::Item item;
        item.text = din_list.at(i)->text();
        item.value = din_list.at(i)->subTypeID();
        result.append(item);
    }

    return result;
}

QList<ComboBoxDelegate::Item> VFuncDelegate::fixDinList(uint16_t value) const
{
    QList<ComboBoxDelegate::Item> result;
    QList<VirtualInputSignal*> din_list =controller()->serviceManager()->vdins();
    for (int i = 0; i < din_list.count();i++) {
        if (din_list.at(i)->source()) {
            if (din_list.at(i)->subTypeID() != value)
                continue;
        }
        auto p = controller()->paramsRegistry().element(SP_DIN_VDINFIXED, i / 8);
        if (!p)
            continue;
        if ( !(p->value().toUInt() & (1 << (i % 8))))
            continue;
        ComboBoxDelegate::Item item;
        item.text = din_list.at(i)->text();
        item.value = din_list.at(i)->subTypeID();
        result.append(item);
    }

    return result;
}

QStyledItemDelegate* VFuncDelegate::getDelegate(VFunc::Type type, uint16_t value) const
{
    if (type == VFunc::XCBR_RZA_CNTRL)
        return new IntSpinBoxDelegate(0, 255);

    if (type == VFunc::VDOUT_CONFIRM)
        return new IntSpinBoxDelegate(0, 255);

    if (type == VFunc::XCBR_CNTRL)
        return new ComboBoxDelegate(xcbrList());

    if (type == VFunc::VDIN_CONTROL)
        return new ComboBoxDelegate(dinList(value));

    if (type == VFunc::VDIN_EVENT)
        return new ComboBoxDelegate(dinList(value));

    if (type == VFunc::FIX_VDIN)
        return new ComboBoxDelegate(fixDinList(value));

    if (type == VFunc::CONTROL_SV)
        return new ComboBoxDelegate(on_off);

    if (type == VFunc::ACTIVE_GROUP)
        return new ComboBoxDelegate(groups);

    return nullptr;
}
