#ifndef __CFC_AND_H__
#define __CFC_AND_H__


#include <QObject>
#include <QGraphicsItem>
#include <QDomNode>
#include <QPainter>
#include <QString>
#include <QSizeF>

#include "gui/forms/algorithms/custom/cfc_editor/cfc_node.h"


//===================================================================================================================================================
//	Элемент AND
//===================================================================================================================================================
class CfcAnd : public CfcNode
{
	Q_OBJECT

public:
    CfcAnd(QString id = QString(), QSizeF node_size = QSizeF(), QGraphicsItem* parent = nullptr);
    CfcAnd(QDomNode xml, QGraphicsItem* parent = nullptr);
    CfcAnd(RamNode node, QGraphicsItem* parent = nullptr);

protected:
    void paintElement(QPainter *painter) override;
};


#endif	//	__CFC_AND_H__
