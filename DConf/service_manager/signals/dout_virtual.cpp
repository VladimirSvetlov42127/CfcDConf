#include "dout_virtual.h"


//===================================================================================================================================================
//	Конструктор класса
//===================================================================================================================================================
DoutVirtual::DoutVirtual(DcController* controller, int32_t id, int32_t internalid, uint8_t direction, uint8_t type, uint8_t subtype, const QString &name, uint16_t subtypeID) :
    DoutSignal(controller, id, internalid, direction, type, subtype, name, subtypeID)
{
//    _name_param = controller->getParam(SP_VIRTDOUT_NAME, subtypeID);
}


//===================================================================================================================================================
//	Перегружаемые методы класса
//===================================================================================================================================================
QString DoutVirtual::text() const
{
    QString tmp = QString("[%1] %2");
    return customName().isEmpty() ?  tmp.arg(QString::number(internalID()), name()) : tmp.arg(QString::number(internalID()), customName());
}

QString DoutVirtual::fullText() const
{
    return text();
}
