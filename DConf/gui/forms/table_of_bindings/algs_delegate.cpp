#include "algs_delegate.h"


//===================================================================================================================================================
//	Подключение библиотек QT
//===================================================================================================================================================
#include <QApplication>

#include <dpc/helper.h>

//===================================================================================================================================================
//	Подключение модулей проекта
//===================================================================================================================================================
#include "service_manager/services/service_input.h"
#include "service_manager/services/service_output.h"


//===================================================================================================================================================
//	Конструктор класса
//===================================================================================================================================================
AlgsDelegate::AlgsDelegate(QObject* parent) : IBindingDelegate(parent)
{
}


//===================================================================================================================================================
//	Перегружаемые методы класса
//===================================================================================================================================================
void AlgsDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    //	Проверка входных данных
	if (!index.isValid())
		return;
    if (!index.data(IO_FLAG).isValid())
        return;

    //	Получение данных модели
    QString name = QString();
    bool hasWarning = false;
    uint8_t flag = index.data(IO_FLAG).toInt();
    if (flag == 0) {
        auto service_data = Dpc::toPtr<ServiceInput>(index.data(SERVICE_DATA));
        if (service_data->source()) {
            name = service_data->source()->fullText();
            auto vdin = service_data->source()->to<VirtualInputSignal>();
            if (vdin && !vdin->source())
                hasWarning = true;
        }
    }
    if (flag == 1) {
        auto service_data = Dpc::toPtr<ServiceOutput>(index.data(SERVICE_DATA));
        if (service_data->target())
            name = service_data->target()->fullText();
    }

    //	Вывод кнопки привязки
    QRect rectangle = option.rect;
    QStyleOptionButton button;
    button.rect = QRect(rectangle.left() + 5, rectangle.top(), 20, 20);

    name == QString() ? button.icon = QIcon(":/icons/connect_no.svg") : button.icon = QIcon(":/icons/connect_yes.svg");
    currentRow() == index.row() ? button.state = QStyle::State_Sunken | QStyle::State_Enabled : button.state = QStyle::State_Raised | QStyle::State_Enabled;
    button.iconSize = QSize(16, 16);
    QApplication::style()->drawControl(QStyle::CE_PushButton, &button, painter);

	//	Опции вывода
    QStyleOptionViewItem delegate_option = option;
    delegate_option.text = name;
    delegate_option.rect = QRect(rectangle.left() + 30, rectangle.top() - 1, rectangle.width() - 10, rectangle.height());
    delegate_option.palette.setBrush(QPalette::HighlightedText, Qt::black);
    if (hasWarning)
        delegate_option.backgroundBrush = QColor(Qt::yellow);
    QApplication::style()->drawControl(QStyle::CE_ItemViewItem, &delegate_option, painter);

	return;
}
