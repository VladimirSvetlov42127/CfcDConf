#ifndef __CFC_RS_TRIGGER_H__
#define __CFC_RS_TRIGGER_H__


#include <QObject>
#include <QGraphicsItem>
#include <QDomNode>
#include <QPainter>
#include <QString>
#include <QSizeF>

#include "gui/forms/algorithms/custom/cfc_editor/cfc_node.h"


//===================================================================================================================================================
//	RS Триггер
//===================================================================================================================================================
class CfcRsTrigger : public CfcNode
{
	Q_OBJECT

public:
    CfcRsTrigger(QString id = QString(), QSizeF node_size = QSizeF(), QGraphicsItem* parent = nullptr);
    CfcRsTrigger(QDomNode xml, QGraphicsItem* parent = nullptr);
    CfcRsTrigger(RamNode node, QGraphicsItem* parent = nullptr);

protected:
    void paintElement(QPainter* painter) override;
};


#endif	//	__CFC_RS_TRIGGER_H__
