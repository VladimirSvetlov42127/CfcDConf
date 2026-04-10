#ifndef __CFC_BO_H__
#define __CFC_BO_H__


#include <QObject>
#include <QGraphicsItem>
#include <QDomNode>
#include <QPainter>
#include <QString>
#include <QSizeF>

#include "gui/forms/algorithms/custom/cfc_editor/cfc_node.h"
#include "service_manager/services/alg_cfc/cfc_service_output.h"


//===================================================================================================================================================
//	Дискретный выход
//===================================================================================================================================================
class CfcBO : public CfcNode
{
	Q_OBJECT

public:
    CfcBO(QString id = QString(), QSizeF node_size = QSizeF(), CfcServiceOutput* output = nullptr, QGraphicsItem* parent = nullptr);
    CfcBO(QDomNode xml, CfcServiceOutput* output = nullptr, QGraphicsItem* parent = nullptr);
    CfcBO(RamNode node, CfcServiceOutput* output = nullptr, QGraphicsItem* parent = nullptr);

    CfcServiceOutput* cfcOutput() const { return _output; }
    void setCfcOutput(CfcServiceOutput* output) { _output = output; };
    virtual QList<NodeParam> paramsList() const override;

protected:
    void paintElement(QPainter* painter) override;

private:
    void outputReset();

    //	Свойства класса
    CfcServiceOutput* _output;
};


#endif	//	__CFC_BO_H__
