#include "ibinding_delegate.h"


//===================================================================================================================================================
//	Подключение библиотек QT
//===================================================================================================================================================
#include <QDebug>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QApplication>
#include <QEvent>
#include <QMouseEvent>
#include <QPainter>
#include <QStyleOptionViewItem>
#include <QVariant>

#include <dpc/helper.h>

//===================================================================================================================================================
//	Подключение модулей проекта
//===================================================================================================================================================
// #include "service_manager/services/service_input.h"
// #include "service_manager/services/service_output.h"


//===================================================================================================================================================
//	Конструктор класса
//===================================================================================================================================================
IBindingDelegate::IBindingDelegate(QObject* parent) : QItemDelegate(parent)
{
	//	Свойства класса
    _current_row = -1;
}


//===================================================================================================================================================
//	Перегружаемые методы класса
//===================================================================================================================================================
// void IBindingDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
// {
// 	//	Проверка элемента
// 	if (!index.isValid())
// 		return;
// 	QStandardItemModel* model = (QStandardItemModel*)index.model();
// 	QStandardItem* item = model->itemFromIndex(index);


//     if (!item->data(Qt::UserRole).isValid())
// 		return;

//     //	Получение данных модели
//     QString name = QString();
//     uint8_t flag = item->data(Qt::UserRole).toInt();
//     if (flag == 0) {
//         auto service_data = Dpc::toPtr<ServiceInput>(item->data(Qt::UserRole + 1));
//         if (service_data->source()) name = service_data->source()->fullText();
//     }
//     if (flag == 1) {
//         auto service_data = Dpc::toPtr<ServiceOutput>(item->data(Qt::UserRole + 1));
//         if (service_data->target()) name = service_data->target()->fullText();
//     }

//     //	Вывод кнопки привязки
//     QRect rectangle = option.rect;
//     QStyleOptionButton button;
//     button.rect = QRect(rectangle.left() + 5, rectangle.top(), 20, 20);

//     name == QString() ? button.icon = QIcon(":/icons/connect_no.svg") : button.icon = QIcon(":/icons/connect_yes.svg");
//     _current_row == index.row() ? button.state = QStyle::State_Sunken | QStyle::State_Enabled : button.state = QStyle::State_Raised | QStyle::State_Enabled;
//     button.iconSize = QSize(16, 16);
//     QApplication::style()->drawControl(QStyle::CE_PushButton, &button, painter);

// 	//	Опции вывода
//     QStyleOptionViewItem delegate_option = option;
//     delegate_option.text = name;
//     delegate_option.rect = QRect(rectangle.left() + 30, rectangle.top() - 1, rectangle.width() - 10, rectangle.height());
//     delegate_option.palette.setBrush(QPalette::HighlightedText, Qt::black);
//     QApplication::style()->drawControl(QStyle::CE_ItemViewItem, &delegate_option, painter);
// }

void IBindingDelegate::updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	editor->setGeometry(option.rect);
}

bool IBindingDelegate::editorEvent(QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& option, const QModelIndex& index)
{
	//	Проверка нажатия кнопки мыши
	if (event->type() != QEvent::MouseButtonPress && event->type() != QEvent::MouseButtonRelease) 
		return true;

	//	Нажатие кнопки мыши
	if (event->type() == QEvent::MouseButtonPress) {
		_current_row = index.row();
		return true;
	}

	//	Отпускание кнопки мыши
	if (event->type() == QEvent::MouseButtonRelease) {
        setCurrentRow(-1);

		//	Координаты мышки
		QMouseEvent* evnt = (QMouseEvent*) event;
		int click_x = evnt->x();
		int click_y = evnt->y();

		//	Область срабатывания мышки
		QRect rectangle = option.rect;
		int x = rectangle.left() + 5;
		int y = rectangle.top();
		if ((click_x > x && click_x < x + 25) && (click_y > y && click_y < y + 25))
            emit onClicked(index);
		return true;
	}

	return true;
}

