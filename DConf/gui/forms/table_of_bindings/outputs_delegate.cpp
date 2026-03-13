#include "outputs_delegate.h"


//===================================================================================================================================================
//	Подключение библиотек QT
//===================================================================================================================================================
#include <QApplication>

#include <dpc/helper.h>

//===================================================================================================================================================
//	Подключение модулей проекта
//===================================================================================================================================================
#include "service_manager/signals/output_signal.h"
#include "service_manager/signals/virtual_input_signal.h"


//===================================================================================================================================================
//	Конструктор класса
//===================================================================================================================================================
OutputsDelegate::OutputsDelegate(QObject* parent) : IBindingDelegate(parent)
{
}


//===================================================================================================================================================
//	Перегружаемые методы класса
//===================================================================================================================================================
void OutputsDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	//	Проверка элемента
	if (!index.isValid())
		return;
    if (!index.data(SERVICE_DATA).isValid())
        return;

    //  Получение параметров привязки
    QString name = QString();
    auto output = Dpc::toPtr<OutputSignal>(index.data(SERVICE_DATA));
    bool hasWarning = false;
    if (output->source()) {
        name = output->source()->fullText();
        auto vdin = output->source()->to<VirtualInputSignal>();
        if (vdin && !vdin->source())
            hasWarning = true;
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

