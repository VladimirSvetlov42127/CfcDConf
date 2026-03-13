#include "QxdTableView.h"

#include <QDebug>
#include <QMouseEvent>
#include <QScrollBar>
#include <QMenu>

#include <dpc/gui/widgets/qxd_tableview/QxdHeaderView.h>
#include <dpc/gui/widgets/qxd_tableview/QxdProxyModel.h>
#include <dpc/gui/widgets/qxd_tableview/QxdHeaderViewMenu.h>

#include <dpc/gui/widgets/qxd_tableview/filters/CompareFilterEditor.h>
#include <dpc/gui/widgets/qxd_tableview/filters/TextFilterEditor.h>
#include <dpc/gui/widgets/qxd_tableview/filters/ListFilterEditor.h>

namespace {
    const QString TEXT_FILTER_ACTION = "Фильтровать по выбранным значениям";
    const QString TEXT_CLEAR_FILTER = "Очистить фильтр со столбца";

    enum ActionDataRole {
        UserIdxRole,
        UserOnlySingleRole,
        ColumnRole,
        SelectedRowsRole
    };

    class QxdAction : public QAction
    {
    public:
        using QAction::QAction;
        using TypeCheckFunc = std::function<bool(int, const QModelIndexList&)>;

        bool checkIfAvailable(int column, const QModelIndexList &rows)
        {
            setData(ColumnRole, column);
            setData(SelectedRowsRole, QVariant::fromValue<QModelIndexList>(rows));

            if (m_checkFunc) {
                auto isAvailable = m_checkFunc(column, rows);
                setEnabled(isAvailable);
                return isAvailable;
            }

            setEnabled(true);
            return true;
        }

        void setData(int role, const QVariant &data) { m_data[role] = data;}
        QVariant data(int role) const { return m_data[role]; }
        void setCheckFunc(TypeCheckFunc func) { m_checkFunc = func; }
    private:
        QMap<int, QVariant> m_data;
        TypeCheckFunc m_checkFunc;
    };
}

QxdTableView::QxdTableView(QWidget *parent) :
    QTableView(parent),
    m_proxyModel(new QxdProxyModel(this)),
    m_header(new QxdHeaderView(this))
{
    setContextMenuPolicy(Qt::CustomContextMenu);
    setSelectionMode(QAbstractItemView::ExtendedSelection);

	setHorizontalHeader(m_header);
    m_header->installEventFilter(this);
    connect(m_header, &QxdHeaderView::filterButtonClicked, this, &QxdTableView::onHeaderFilterButtonClicked);
    connect(m_proxyModel, &QxdProxyModel::filteredCountChanged, this, &QxdTableView::filteredCountChanged);
    connect(m_proxyModel, &QxdProxyModel::sorted, this, &QxdTableView::moveToSelected);
    connect(m_proxyModel, &QxdProxyModel::filtersInvalidated, this, &QxdTableView::moveToSelected);
    connect(m_proxyModel, &QxdProxyModel::modelReset, this, &QTableView::scrollToTop);
    connect(verticalScrollBar(), &QScrollBar::valueChanged, this, [this] (int value) {
        if (verticalScrollBar()->maximum() == value)
            fetchMore();
    });

    auto checkForFetchMore = [this]() {
        if (m_proxyModel->rowCount() < this->size().height() / verticalHeader()->defaultSectionSize())
            fetchMore();
    };
    connect(m_proxyModel, &QxdProxyModel::rowsInserted, this, checkForFetchMore);
    connect(m_proxyModel, &QxdProxyModel::filtersInvalidated, this, checkForFetchMore);

    connect(this, &QTableView::customContextMenuRequested, this, &QxdTableView::onContextMenuRequested);

    auto filterAction = new QxdAction(m_proxyModel->filterIcon(), TEXT_FILTER_ACTION, this);
    connect(filterAction, &QAction::triggered, this, &QxdTableView::onFilterAction);
    filterAction->setCheckFunc([=](int column, const QModelIndexList &list) { return m_proxyModel->getFilter(column); });
    m_contextMenuActions << filterAction;
    m_filterAction = filterAction;

    auto clearFilterAction = new QxdAction(QIcon(":/clear-filter.png"), TEXT_CLEAR_FILTER, this);
    connect(clearFilterAction, &QAction::triggered, this, &QxdTableView::onClearFilterAction);
    clearFilterAction->setCheckFunc([=](int column, const QModelIndexList &) { auto filter = m_proxyModel->getFilter(column); return filter && filter->isEnabled(); });
    m_contextMenuActions << clearFilterAction;
}

QxdTableView::~QxdTableView()
{
}

void QxdTableView::setModel(QAbstractItemModel * model)
{
    if (model == m_proxyModel->sourceModel())
        return;

    m_proxyModel->setSourceModel(model);
    QTableView::setModel(m_proxyModel);
}

void QxdTableView::setFilter(int column, Qxd::FilterType type, QVariant::Type dataType)
{
    QxdIFilter *filter = nullptr;
	switch (type)
	{
    case Qxd::None:
		break;
    case Qxd::List:
        filter = new ListFilter(dataType);
		break;
    case Qxd::Numeric:
		filter = new NumericFilter;
		break;
    case Qxd::Time:
        filter = new TimeFilter;
        break;
    case Qxd::Date:
        filter = new DateFilter;
        break;
    case Qxd::DateTime:
        filter = new DateTimeFilter;
		break;
    case Qxd::Text:
		filter = new TextFilter;
		break;
	default:
		qDebug() << "Unknown filter type" << type;
		return;
	}

	m_proxyModel->addFilter(column, filter);
    update();
}

void QxdTableView::setSortRole(int role)
{
    m_proxyModel->setSortRole(role);
}

void QxdTableView::setFilterRole(int role)
{
    m_proxyModel->setFilterRole(role);
}

void QxdTableView::setSortCaseSensitivity(Qt::CaseSensitivity cs)
{
    m_proxyModel->setSortCaseSensitivityX(cs);
}

//void QxdTableView::setUseSourceSort(const QSet<int> &columns)
//{
//    m_proxyModel->setUseSourceSort(columns);
//}

void QxdTableView::setFilterIcon(const QIcon &icon)
{
    m_proxyModel->setFilterIcon(icon);
    m_filterAction->setIcon(icon);
}

void QxdTableView::addUserContextMenuItem(int menuItemIdx, const QString &text, bool onlySingle, const QIcon &icon)
{
    auto action = new ::QxdAction(icon, text, this);
    if (text.isEmpty())
        action->setSeparator(true);
    action->setData(UserIdxRole, menuItemIdx);
    action->setData(UserOnlySingleRole, onlySingle);
    m_userContextMenuActions[menuItemIdx] = action;
}

bool QxdTableView::eventFilter(QObject *o, QEvent *e)
{
    if (o == m_header && e->type() == QEvent::ContextMenu) {
        showFrame(new QxdHeaderViewMenu(m_header, this), m_header->mapToParent(static_cast<QContextMenuEvent*>(e)->pos()));
    }

    return false;
}

void QxdTableView::setDynamicSortFilter(bool enable)
{
    m_proxyModel->setDynamicSortFilter(enable);
}

void QxdTableView::onHeaderFilterButtonClicked(int idx)
{
    QxdIFilter *filter = m_proxyModel->getFilter(idx);
	if (!filter)
        return;

	FilterEditor *editor = nullptr;
    switch (filter->type()) {
    case Qxd::Numeric:
    case Qxd::Time:
    case Qxd::Date:
    case Qxd::DateTime:
    {
        editor = new CompareFilterEditor(static_cast<CompareFilter*>(filter), QVariant(), QVariant(), this);
        break;
    }
    case Qxd::List:
    {
        auto acceptFunc = [this, idx](int row) { return m_proxyModel->acceptedByFilters(row, QModelIndex(), idx); };
        editor = new ListFilterEditor(static_cast<ListFilter*>(filter), m_proxyModel->sourceModel(),
                                      idx, m_proxyModel->filterRole(), acceptFunc, this);
    }
        break;
    case Qxd::Text:
        editor = new TextFilterEditor(static_cast<TextFilter*>(filter), this);
        break;
    default:
        qDebug() << "Failed create editor for filter type" << filter->type();
        return;
    }

    connect(editor, &FilterEditor::accepted, m_proxyModel, &QxdProxyModel::invalidateFilter);    
	QPoint centerPos = m_header->mapToParent(QPoint(m_header->sectionViewportPosition(idx) + m_header->sectionSize(idx) - 10, m_header->height()));
    showFrame(editor, centerPos);
}

void QxdTableView::moveToSelected()
{
    QModelIndex selected = this->selectionModel()->selectedRows().value(0);
    if (selected.isValid())
        scrollTo(selected, QAbstractItemView::PositionAtCenter);
    else
        scrollToTop();

    setFocus();
}

void QxdTableView::fetchMore()
{
    if (!m_proxyModel->sourceModel()->canFetchMore(QModelIndex()))
        return;

    m_proxyModel->sourceModel()->fetchMore(QModelIndex());
}

void QxdTableView::onContextMenuRequested(const QPoint &pos)
{
    auto indexUnderCursor = m_proxyModel->mapToSource(indexAt(pos));
    if (!indexUnderCursor.isValid()) {
        showFrame(new QxdHeaderViewMenu(m_header, this), m_header->mapToParent(pos));
        return;
    }

    int selectedColumn = indexUnderCursor.column();
    QList<QModelIndex> selectedRowsIndexs; //QSet?
    for(auto &it: selectedIndexes())
        if (it.column() == selectedColumn)
            selectedRowsIndexs.append(m_proxyModel->mapToSource(it));

    if (!selectedRowsIndexs.contains(indexUnderCursor))
        return;

    QMenu menu;
    for(auto &it: m_contextMenuActions) {
        auto action = dynamic_cast<QxdAction*>(it);
        if (action)
            action->checkIfAvailable(selectedColumn, selectedRowsIndexs);

        menu.addAction(it);
    }

    menu.addSeparator();
    for(auto &it: m_userContextMenuActions) {
        auto action = static_cast<QxdAction*>(it);
        bool onlySingle = action->data(UserOnlySingleRole).toBool();
        if (onlySingle && selectedRowsIndexs.count() != 1)
            continue;

        menu.addAction(action);
    }

    if (menu.actions().isEmpty())
        return;

    connect(&menu, &QMenu::aboutToShow, this, &QxdTableView::contextMenuAboutToShow);
    connect(&menu, &QMenu::aboutToHide, this, &QxdTableView::contextMenuAboutToHide);
    auto selectedAction = menu.exec(viewport()->mapToGlobal(pos));
    if (!selectedAction || m_contextMenuActions.contains(selectedAction))
        return;

    QList<int> selectedRows;
    for(auto &it: selectedRowsIndexs)
        selectedRows << it.row();
    int menuItemIdx = static_cast<QxdAction*>(selectedAction)->data(UserIdxRole).toInt();
    emit userContextMenuClicked(menuItemIdx, selectedColumn, selectedRows);
}

void QxdTableView::onFilterAction()
{
    auto action = dynamic_cast<QxdAction*>(sender());
    if (!action)
        return;

    QList<QVariant> values;
    for(auto &it: action->data(SelectedRowsRole).value<QModelIndexList>())
        values << it.data();

    auto filter = m_proxyModel->getFilter(action->data(ColumnRole).toInt());
    filter->setEqualTo(values);
    m_proxyModel->invalidateFilter();
}

void QxdTableView::onClearFilterAction()
{
    auto action = dynamic_cast<QxdAction*>(sender());
    if (!action)
        return;

    auto column = action->data(ColumnRole).toInt();
    m_proxyModel->resetFilter(column);
    m_proxyModel->invalidateFilter();
}

void QxdTableView::showFrame(QxdFrame *frame, const QPoint &pos)
{
    frame->setAttribute(Qt::WA_DeleteOnClose);
    connect(frame, &QxdFrame::lostFocus, frame, &QxdFrame::close);
    int x = pos.x() - frame->width() / 2;
    if (x < this->viewport()->x())
        x = this->viewport()->x();
    if (x + frame->width() > this->viewport()->width())
        x = this->viewport()->width() - frame->width();
    QPoint p(x, pos.y());
    frame->setGeometry(QRect(p, frame->geometry().size()));
    frame->show();
    frame->setFocus();
}
