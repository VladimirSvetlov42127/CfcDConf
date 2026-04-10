#include "cin_signal.h"

#include "data_model/dc_controller.h"

CinSignal::CinSignal(const Config &config)
    : Signal{config}
{

}

bool CinSignal::init(DcController *config)
{
    m_iec101SelectFlag = config->paramsRegistry().elementBit(SP_SELECT_MASK, internalID(), 2);
    m_iec101Group1Flag = config->paramsRegistry().elementBit(SP_GROUP1LIST, internalID(), 2);
    m_iec101Group2Flag = config->paramsRegistry().elementBit(SP_GROUP2LIST, internalID(), 2);
    m_iec101BackgroundFlag = config->paramsRegistry().elementBit(SP_BACKGROUNDLIST, internalID(), 2);
    m_spodesSelectFlag = config->paramsRegistry().elementBit(SP_SPODES_MASK, internalID(), 2);

    return Signal::init(config);
}
