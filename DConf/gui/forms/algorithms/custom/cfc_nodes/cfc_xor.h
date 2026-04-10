#ifndef __CFC_XOR_H__
#define __CFC_XOR_H__


#include <QObject>
#include <QGraphicsItem>
#include <QDomNode>
#include <QPainter>
#include <QString>
#include <QSizeF>

#include "gui/forms/algorithms/custom/cfc_editor/cfc_node.h"


//===================================================================================================================================================
//	Элемент XOR
//===================================================================================================================================================
class CfcXor : public CfcNode
{
	Q_OBJECT

public:
    CfcXor(QString id = QString(), QSizeF node_size = QSizeF(), QGraphicsItem* parent = nullptr);
    CfcXor(QDomNode xml, QGraphicsItem* parent = nullptr);
    CfcXor(RamNode node, QGraphicsItem* parent = nullptr);

protected:
    void paintElement(QPainter* painter) override;
};


#endif	//	__CFC_XOR_H__
