#ifndef QXDTABLEVIEW_H
#define QXDTABLEVIEW_H

#include <QTableView>

#include <dpc/dpc_global.h>
#include <dpc/gui/widgets/qxd_tableview/QxdFilter.h>

class QxdHeaderView;
class QxdProxyModel;
class QxdFrame;

class DPC_EXPORT QxdTableView : public QTableView
{
	Q_OBJECT

public:
    QxdTableView(QWidget *parent = nullptr);
    virtual ~QxdTableView();

	virtual void setModel(QAbstractItemModel *model) override;

    // Установка типа фильтра на столбец, с возможностью указания типа данных
    void setFilter(int column, Qxd::FilterType type, QVariant::Type dataType = QVariant::Invalid);

    // Установка роли для сортировки
    void setSortRole(int role);

    // Установка роли для фильтрации
    void setFilterRole(int role);

     // Установка чуствительности регистра при сортировке строк
    void setSortCaseSensitivity(Qt::CaseSensitivity cs);

    //void setUseSourceSort(const QSet<int> &columns);

    // Задание иконки фильтра
    void setFilterIcon(const QIcon &icon);

    // Добавить пункт контекстного меню, onlySingle = отображать пункт меню только при одиночном выделении
    // Если text будет пустым, добавиться разделитель
    void addUserContextMenuItem(int menuItemIdx, const QString &text = QString(), bool onlySingle = true, const QIcon &icon = QIcon());

    virtual bool eventFilter(QObject *o, QEvent *e) override;

signals:
    // уведомление об изменение количества отфильтрованных записей
    void filteredCountChanged(int count);

    // Перед вызовом контекстного меню
    void contextMenuAboutToShow();

    // Выбор пункта меню
    void userContextMenuClicked(int menuItemIdx, int selectedColumn, const QList<int> selectedRows);

    // После вызова контекстного меню
    void contextMenuAboutToHide();

public slots:
    void setDynamicSortFilter(bool enable);    

private slots:
    void onHeaderFilterButtonClicked(int idx);
    void moveToSelected();
    void fetchMore();
    void onContextMenuRequested(const QPoint &pos);

    void onFilterAction();
    void onClearFilterAction();

private:
    void showFrame(QxdFrame *frame, const QPoint &pos);

private:
    QxdProxyModel *m_proxyModel;
    QxdHeaderView *m_header;

    QList<QAction*> m_contextMenuActions;
    QAction* m_filterAction;

    QMap<int, QAction*> m_userContextMenuActions;
};

#endif // QXDTABLEVIEW_H
