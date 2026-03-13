#include "QxdHeaderView.h"

#include <QMouseEvent>
#include <QPainter>
#include <QDebug>

#include <dpc/gui/widgets/qxd_tableview/QxdProxyModel.h>

static const int gButtonWidth = 14;

QxdHeaderView::QxdHeaderView(QWidget *parent) :
	QHeaderView(Qt::Horizontal, parent),
	m_sectionMouseOver(-1),
	m_sectionButtonPressed(-1)
{
	setSectionsClickable(true);
	setSectionsMovable(true);
}

QxdHeaderView::~QxdHeaderView()
{
}

void QxdHeaderView::setModel(QAbstractItemModel *model)
{
    QAbstractItemModel *current = this->model();
    if (current == model)
        return;

    QxdProxyModel *proxy = dynamic_cast<QxdProxyModel*>(current);
    if (proxy) {
        disconnect(proxy, &QxdProxyModel::filtersAboutToBeInvalidate, this, &QxdHeaderView::onFiltersAboutToBeInvalidate);
        disconnect(proxy, &QxdProxyModel::filtersInvalidated, this, &QxdHeaderView::onFiltersInvalidated);
    }
    proxy = dynamic_cast<QxdProxyModel*>(model);
    if (proxy) {
        connect(proxy, &QxdProxyModel::filtersAboutToBeInvalidate, this, &QxdHeaderView::onFiltersAboutToBeInvalidate);
        connect(proxy, &QxdProxyModel::filtersInvalidated, this, &QxdHeaderView::onFiltersInvalidated);
    }
    QHeaderView::setModel(model);
}

void QxdHeaderView::mousePressEvent(QMouseEvent * e)
{
    if (e->buttons() & Qt::LeftButton)
        m_sectionButtonPressed = sectionButtonAt(e->pos());
	QHeaderView::mousePressEvent(e);
}

void QxdHeaderView::mouseMoveEvent(QMouseEvent * e)
{
	QHeaderView::mouseMoveEvent(e);
	m_sectionMouseOver = logicalIndexAt(e->pos());
	if (sectionButtonAt(e->pos()) >= 0)
		setCursor(Qt::PointingHandCursor);
}

void QxdHeaderView::mouseReleaseEvent(QMouseEvent * e)
{
	if (m_sectionButtonPressed >= 0 && m_sectionButtonPressed == sectionButtonAt(e->pos())) 
		emit filterButtonClicked(m_sectionButtonPressed);
	else
		QHeaderView::mouseReleaseEvent(e);
	m_sectionButtonPressed = -1;	
}

void QxdHeaderView::leaveEvent(QEvent * e)
{
	m_sectionMouseOver = -1;
	QHeaderView::leaveEvent(e);
}

void QxdHeaderView::paintSection(QPainter * painter, const QRect & rect, int logicalIndex) const
{
	QHeaderView::paintSection(painter, rect, logicalIndex);

    QxdProxyModel *prModel = dynamic_cast<QxdProxyModel*>(model());
    if (!prModel || !prModel->getFilter(logicalIndex)) return;

    QStyleOptionHeader opt;
    initStyleOption(&opt);
    QStyle::State state = QStyle::State_None;
    if (isEnabled()) state |= QStyle::State_Enabled;
    if (window()->isActiveWindow()) state |= QStyle::State_Active;
    if (sectionsClickable()) if (logicalIndex == m_sectionMouseOver) state |= QStyle::State_MouseOver; 
   
    opt.state = state;
    opt.rect = rect;
    opt.rect.setLeft(rect.right() - ::gButtonWidth);
    painter->setClipRect(opt.rect);
    style()->drawControl(QStyle::CE_HeaderSection, &opt, painter, this);
    //style()->drawPrimitive(QStyle::PE_IndicatorArrowDown, &opt, painter, this);

    //  Вывод стрелки фильтра вместо примитив
    painter->save();
    painter->setPen(0xCBD5DA);
    painter->setBrush(QBrush(0xCBD5DA));

    // Создание полигона в форме стрелки 
    QPolygon arrow;
    int arrowWidth = 8;         // Ширина стрелки, можно настроить 
    int arrowHeight = 6;        // Высота стрелки, можно настроить 
    QPoint basePoint = opt.rect.center(); // Точка отсчета стрелки (центр) 
    arrow << QPoint(basePoint.x() - arrowWidth / 2, basePoint.y() - arrowHeight / 2)
        << QPoint(basePoint.x() + arrowWidth / 2, basePoint.y() - arrowHeight / 2)
        << QPoint(basePoint.x(), basePoint.y() + arrowHeight / 2);

    // Рисование стрелки 
    painter->drawPolygon(arrow);
    painter->restore();    
    
    return;
}

void QxdHeaderView::onFiltersAboutToBeInvalidate()
{
    m_hiddenSections.clear();
    for(int i = 0; i < count(); i++) if (isSectionHidden(i)) m_hiddenSections.append(i);
}

void QxdHeaderView::onFiltersInvalidated()
{
    for(int i = 0; i < m_hiddenSections.count(); i++) setSectionHidden(m_hiddenSections.at(i), true);
}

int QxdHeaderView::sectionButtonAt(const QPoint & p) const
{
	int section = logicalIndexAt(p);
    QxdProxyModel *prModel = dynamic_cast<QxdProxyModel*>(model());
	if (prModel && prModel->getFilter(section)) {
		int sectionRight = sectionViewportPosition(section) + sectionSize(section) - 4;
		int sectionLeft = sectionRight - ::gButtonWidth;
		if (sectionLeft <= p.x() && p.x() < sectionRight) return section; }

	return -1;
}
