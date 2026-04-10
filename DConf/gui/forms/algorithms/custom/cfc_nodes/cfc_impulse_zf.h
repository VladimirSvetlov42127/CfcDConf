#ifndef __CFC_IMPULSE_ZF_H__
#define __CFC_IMPULSE_ZF_H__


#include <QObject>
#include <QGraphicsItem>
#include <QDomNode>
#include <QPainter>
#include <QString>
#include <QSizeF>

#include "gui/forms/algorithms/custom/cfc_editor/cfc_node.h"


//===================================================================================================================================================
//	Элемент Импульс по заднему фронту
//===================================================================================================================================================
class CfcImpulseZF : public CfcNode
{
	Q_OBJECT

public:
    CfcImpulseZF(QString id = QString(), QSizeF node_size = QSizeF(), QGraphicsItem* parent = nullptr);
    CfcImpulseZF(QDomNode xml, QGraphicsItem* parent = nullptr);
    CfcImpulseZF(RamNode node, QGraphicsItem* parent = nullptr);

protected:
    void paintElement(QPainter* painter) override;
};

#endif	//	__CFC_IMPULSE_ZF_H__
