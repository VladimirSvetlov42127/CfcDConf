#ifndef __CFC_RETURN_DELAY_H__
#define __CFC_RETURN_DELAY_H__


#include <QObject>
#include <QGraphicsItem>
#include <QDomNode>
#include <QPainter>
#include <QString>
#include <QSizeF>

#include "gui/forms/algorithms/custom/cfc_editor/cfc_node.h"


//===================================================================================================================================================
//	Задержка возврата
//===================================================================================================================================================
class CfcReturnDelay : public CfcNode
{
	Q_OBJECT

public:
    CfcReturnDelay(QString id = QString(), QSizeF node_size = QSizeF(), QGraphicsItem* parent = nullptr);
    CfcReturnDelay(QDomNode xml, QGraphicsItem* parent = nullptr);
    CfcReturnDelay(RamNode node, QGraphicsItem* parent = nullptr);

protected:
    void paintElement(QPainter* painter) override;
};


#endif	//	__CFC_RETURN_DELAY_H__
