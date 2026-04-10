#ifndef __CFC_NOT_H__
#define __CFC_NOT_H__


#include <QObject>
#include <QGraphicsItem>
#include <QDomNode>
#include <QPainter>
#include <QString>
#include <QSizeF>

#include "gui/forms/algorithms/custom/cfc_editor/cfc_node.h"


//===================================================================================================================================================
//	Элемент Not
//===================================================================================================================================================
class CfcNot : public CfcNode
{
	Q_OBJECT

public:
    CfcNot(QString id = QString(), QSizeF node_size = QSizeF(), QGraphicsItem* parent = nullptr);
    CfcNot(QDomNode xml, QGraphicsItem* parent = nullptr);
    CfcNot(RamNode node, QGraphicsItem* parent = nullptr);

protected:
    void paintElement(QPainter* painter) override;
};


#endif	//	__CFC_NOT_H__
