#include "dout_signal.h"


DoutSignal::DoutSignal(DcController* controller, int32_t id, int32_t internalid, uint8_t direction, uint8_t type, uint8_t subtype, const QString &name, uint16_t subtypeID) :
    Signal { id, internalid, direction, type, subtype, name, subtypeID }
{
//    _impulse_param = controller->getParam(SP_DOUT_SAMPLTIM, internalid);
}

