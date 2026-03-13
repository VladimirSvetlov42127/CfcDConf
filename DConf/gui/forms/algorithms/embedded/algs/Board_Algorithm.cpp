#include "Board_Algorithm.h"

Board_Algorithm::Board_Algorithm(BoardTypes types, int32_t mainParamAddr, int32_t mainParamIdx, bool useMainParamAsSwitcher) :
	IAlgorithm(mainParamAddr, mainParamIdx, useMainParamAsSwitcher),
    m_algBoardTypes(types)
{
}

bool Board_Algorithm::checkForAvailable() const
{
    BoardTypes controllerBoardTypes = BoardType::Unknown;
    for(auto &board: controller()->boards()) {
        auto boardType = board->type().toUpper();
        if ("DRZA_AFE" == boardType)
            controllerBoardTypes |= BoardType::AFE;
        if ("DRZA_AFE7" == boardType)
            controllerBoardTypes |= BoardType::AFE7;
        if ("DRZA_AFE11" == boardType)
            controllerBoardTypes |= BoardType::AFE11;
        if ("DRZA_AFE47" == boardType)
            controllerBoardTypes |= BoardType::AFE47;
    }

    if (controllerBoardTypes == BoardType::Unknown)
        controllerBoardTypes = BoardType::NoBoard;

    return controllerBoardTypes & m_algBoardTypes;
}
