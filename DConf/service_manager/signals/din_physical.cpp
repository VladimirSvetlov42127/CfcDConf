#include "din_physical.h"


//===================================================================================================================================================
//	Конструктор класса
//===================================================================================================================================================
DinPhysical::DinPhysical(DcController* controller, int32_t id, int32_t internalid, uint8_t direction, uint8_t type, uint8_t subtype, const QString &name, uint16_t subtypeID, int board) :
    DinSignal(controller, id, internalid, direction, type, subtype, name, subtypeID)
{
    _board = board;
}

