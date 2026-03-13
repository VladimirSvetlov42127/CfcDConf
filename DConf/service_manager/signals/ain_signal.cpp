#include "ain_signal.h"


AinSignal::AinSignal(DcController* controller, int32_t id, int32_t internalid, uint8_t direction, uint8_t type, uint8_t subtype, const QString &name, uint16_t subtypeID) :
    Signal { id, internalid, direction, type, subtype, name, subtypeID }
{
//    _aperture_type = controller->getParam(SP_AIN_APERTURE_TYPE, internalid);
//    _aperture_value = controller->getParam(SP_AIN_APERTURE_VAL, internalid);
//    _aperture_sens = controller->getParam(SP_AIN_SENSIBILITY, internalid);

//    if (controller->type() == DcController::GSM) {
//        int index = internalid - 4;
//        if (index >= 0) {
//            _aperture_low = controller->getParam(SP_DIN_TO_LOW, index);
//            _aperture_high = controller->getParam(SP_DIN_TO_HIGH, index);
//        }
//    }

//    if (controller->type() == DcController::LT) {
//        int index = internalid - 3;
//        if (index >= 0 && index < 3) {
//            _aperture_low = controller->getParam(SP_AIN_PHYLVL1, index * 2 + 1);
//            _aperture_high = controller->getParam(SP_AIN_PHYLVL1, index * 2);
//        }
//    }
}

