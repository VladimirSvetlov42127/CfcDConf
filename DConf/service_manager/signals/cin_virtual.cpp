#include "cin_virtual.h"


//===================================================================================================================================================
//	Конструктор класса
//===================================================================================================================================================
CinVirtual::CinVirtual(DcController* controller, int32_t id, int32_t internalid, uint8_t direction, uint8_t type, uint8_t subtype, const QString &name, uint16_t subtypeID) :
    CinSignal(id, internalid, direction, type, subtype, name, subtypeID)
{
//    _name_param = controller->getParam(SP_VIRTCIN_NAME, subtypeID);
}


//===================================================================================================================================================
//	Перегружаемые методы класса
//===================================================================================================================================================
QString CinVirtual::text() const
{
    QString tmp = QString("[%1] %2");
    return customName().isEmpty() ?  tmp.arg(QString::number(internalID()), name()) : tmp.arg(QString::number(internalID()), customName());
}

QString CinVirtual::fullText() const
{
    return text();
}
