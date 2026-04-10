#ifndef __CFC_NODE_H__
#define __CFC_NODE_H__


#include <QString>
#include <QDomNode>
#include <QGraphicsItem>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QWidget>
#include <QGraphicsSceneMouseEvent>
#include <QMenu>
#include <QVariant>

#include "cfc_basic_node.h"


//===================================================================================================================================================
//	Вспомогательный класс для всех элементов гибкой логики
//  Вывод графики
//  Обработка внешних событий (мышка)
//===================================================================================================================================================
class CfcNode : public CfcBasicNode
{
public:
    CfcNode(QString id = QString(), QGraphicsItem* parent = nullptr);
    CfcNode(QDomNode xml, QGraphicsItem* parent = nullptr);
    CfcNode(RamNode node, QGraphicsItem* parent = nullptr);

    void editNode();

    QRectF availableRect() const { return QRectF(QPointF(0, 0), size()); }
    virtual void paintElement(QPainter* painter) = 0;
    virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)  override;
    virtual QRectF boundingRect() const override;
    virtual void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
    virtual QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;

private:
    void paintSelected(QPainter* painter);
};

#endif // __CFC_NODE_H__
