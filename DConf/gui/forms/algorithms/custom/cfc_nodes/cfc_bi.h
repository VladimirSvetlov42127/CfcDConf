#ifndef __CFC_BI_H__
#define __CFC_BI_H__


#include <QObject>
#include <QGraphicsItem>
#include <QDomNode>
#include <QPainter>
#include <QString>
#include <QSizeF>

#include "gui/forms/algorithms/custom/cfc_editor/cfc_node.h"
#include "service_manager/services/alg_cfc/cfc_service_input.h"


//===================================================================================================================================================
//	Дискретный вход
//===================================================================================================================================================
class CfcBI : public CfcNode
{
	Q_OBJECT

public:
    CfcBI(QString id = QString(), QSizeF node_size = QSizeF(), CfcServiceInput* input = nullptr, QGraphicsItem* parent = nullptr);
    CfcBI(QDomNode xml, CfcServiceInput* input = nullptr, QGraphicsItem* parent = nullptr);
    CfcBI(RamNode node, CfcServiceInput* input = nullptr, QGraphicsItem* parent = nullptr);

    CfcServiceInput* cfcInput() const { return _input; }
    void setCfcInput(CfcServiceInput* input) { _input = input; };
    QList<NodeParam> paramsList() const override;

protected:
    void paintElement(QPainter* painter) override;

private:
    CfcServiceInput* _input;
};


#endif	//	__CFC_BI_H__
