#pragma once

#include <qstyleditemdelegate.h>

#include <dpc/dpc_global.h>

namespace Dpc::Gui
{
    class DPC_EXPORT ProgressBarDelegate : public QStyledItemDelegate
    {
        Q_OBJECT

    public:
        ProgressBarDelegate(QObject* parent = 0);

        void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;
    };
}