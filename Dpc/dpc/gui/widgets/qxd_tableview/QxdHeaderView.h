#ifndef QXDHEADERVIEW_H
#define QXDHEADERVIEW_H

#include <QHeaderView>

class QxdHeaderView : public QHeaderView
{
	Q_OBJECT

public:
    QxdHeaderView(QWidget *parent = nullptr);
    ~QxdHeaderView();

    void setModel(QAbstractItemModel *model) override;

signals:
	void filterButtonClicked(int logicalIndex);

protected:
	virtual void mousePressEvent(QMouseEvent *e) override;
	virtual void mouseMoveEvent(QMouseEvent *e) override;
	virtual void mouseReleaseEvent(QMouseEvent *e) override;
	virtual void leaveEvent(QEvent *e) override;
	virtual void paintSection(QPainter *painter, const QRect &rect, int logicalIndex) const override;

private slots:
    void onFiltersAboutToBeInvalidate();
    void onFiltersInvalidated();

private:
	int sectionButtonAt(const QPoint &p) const;

private:
	int m_sectionMouseOver;
	int m_sectionButtonPressed;

    QList<int> m_hiddenSections;
};

#endif // QXDHEADERVIEW_H
