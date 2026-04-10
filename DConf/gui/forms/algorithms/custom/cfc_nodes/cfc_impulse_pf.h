#ifndef __CFC_IMPULSE_PF_H__
#define __CFC_IMPULSE_PF_H__


#include <QObject>
#include <QGraphicsItem>
#include <QDomNode>
#include <QPainter>
#include <QString>
#include <QSizeF>

#include "gui/forms/algorithms/custom/cfc_editor/cfc_node.h"


//===================================================================================================================================================
//	Элемент Импульс по переднему фронту
//===================================================================================================================================================
class CfcImpulsePF : public CfcNode
{
	Q_OBJECT

public:
    CfcImpulsePF(QString id = QString(), QSizeF node_size = QSizeF(), QGraphicsItem* parent = nullptr);
    CfcImpulsePF(QDomNode xml, QGraphicsItem* parent = nullptr);
    CfcImpulsePF(RamNode node, QGraphicsItem* parent = nullptr);

protected:
    void paintElement(QPainter* painter) override;
};


#endif	//	__CFC_IMPULSE_PF_H__
