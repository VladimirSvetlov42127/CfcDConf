#ifndef __VFUNC_DELEGATE_H__
#define __VFUNC_DELEGATE_H__


//===================================================================================================================================================
//	Подключение библиотек QT
//===================================================================================================================================================
#include <QObject>
#include <QStyledItemDelegate>
#include <QWidget>
#include <QStyleOptionViewItem>
#include <QModelIndex>
#include <QAbstractItemModel>
#include <QList>

//===================================================================================================================================================
//	Подключение модулей проекта
//===================================================================================================================================================
#include <dpc/gui/delegates/ComboBoxDelegate.h>
#include "data_model/dc_controller.h"
using namespace Dpc::Gui;


//===================================================================================================================================================
//	Описание класса
//===================================================================================================================================================
//	Делегат для вывода параметров виртуальных функций
//  Отображение вывода (комбобокс, спинбокс ...) зависит от выбранной функции
//  Тип выбранной функции хранится в data(role = Qt::UserRole)
//===================================================================================================================================================
class VFuncDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    VFuncDelegate(DcController* controller, QObject* parent = nullptr);

protected:
    //===============================================================================================================================================
    //	Перегружаемые методы класса
    //===============================================================================================================================================
    virtual QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
    virtual void setEditorData(QWidget* editor, const QModelIndex& index) const override;
    virtual void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const override;
    virtual void updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
    virtual void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;

private:
    //===============================================================================================================================================
    //	Вспомогательные методы класса
    //===============================================================================================================================================
    DcController* controller() const {return _controller;}
    void setDelegate(int index, VFunc::Type type, uint16_t value) const;
    QStringList xcbrList() const;
    QList<ComboBoxDelegate::Item> dinList(uint16_t value) const;
    QList<ComboBoxDelegate::Item> fixDinList(uint16_t value) const;
    QStyledItemDelegate* getDelegate(VFunc::Type type, uint16_t value) const;

    //===============================================================================================================================================
    //	Свойства класса
    //===============================================================================================================================================
    struct InnerDelegate {
        VFunc::Type type;
        QStyledItemDelegate* delegate;
    };

    DcController* _controller;
    mutable QList<InnerDelegate> _delegates;
};

#endif // __VFUNC_DELEGATE_H__
