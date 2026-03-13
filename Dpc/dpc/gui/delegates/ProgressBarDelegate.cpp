#include "ProgressBarDelegate.h"

#include <qdebug.h>
#include <qapplication.h>

namespace Dpc::Gui
{
    ProgressBarDelegate::ProgressBarDelegate(QObject* parent)
        : QStyledItemDelegate(parent)
    {
    }

    void ProgressBarDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
    {
        auto error = index.data().toString();
        if (!error.isEmpty()) {
            QStyledItemDelegate::paint(painter, option, index);
            return;
        }

        auto progress = index.data(Qt::UserRole).toInt();
        if (progress < 0) {
            QStyledItemDelegate::paint(painter, option, index);
            return;
        }

        QStyleOptionProgressBar progressBarOption;
        progressBarOption.rect = option.rect;
        progressBarOption.minimum = 0;
        progressBarOption.maximum = 100;
        progressBarOption.progress = progress;
        progressBarOption.text = QString::number(progress) + "%";
        progressBarOption.textVisible = true;

        QApplication::style()->drawControl(QStyle::CE_ProgressBar,
            &progressBarOption, painter);
    }
}
