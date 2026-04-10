#ifndef __CFC_TRIGGER_DELAY_H__
#define __CFC_TRIGGER_DELAY_H__


#include <QObject>
#include <QGraphicsItem>
#include <QDomNode>
#include <QPainter>
#include <QString>
#include <QSizeF>

#include "gui/forms/algorithms/custom/cfc_editor/cfc_node.h"


//===================================================================================================================================================
//	Задержка срабатывания
//===================================================================================================================================================
class CfcTriggerDelay : public CfcNode
{
	Q_OBJECT

public:
    CfcTriggerDelay(QString id = QString(), QSizeF node_size = QSizeF(), QGraphicsItem* parent = nullptr);
    CfcTriggerDelay(QDomNode xml, QGraphicsItem* parent = nullptr);
    CfcTriggerDelay(RamNode node, QGraphicsItem* parent = nullptr);

protected:
    void paintElement(QPainter* painter) override;
};


#endif	//	__CFC_TRIGGER_DELAY_H__
