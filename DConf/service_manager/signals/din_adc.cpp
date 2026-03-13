#include "din_adc.h"


//===================================================================================================================================================
//	Конструктор класса
//===================================================================================================================================================
DinAdc::DinAdc(DcController* controller, int32_t id, int32_t internalid, uint8_t direction, uint8_t type, uint8_t subtype, const QString &name, uint16_t subtypeID) :
    DinSignal(controller, id, internalid, direction, type, subtype, name, subtypeID)
{
}


