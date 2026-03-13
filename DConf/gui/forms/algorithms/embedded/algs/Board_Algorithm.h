#pragma once

#include <gui/forms/algorithms/embedded/IAlgorithm.h>

class Board_Algorithm : public IAlgorithm
{
public:
    enum BoardType : uint16_t {
        Unknown = 0x0,
        NoBoard = 0x1,
        AFE     = 0x2,
        AFE7    = 0x4,
        AFE11   = 0x8,
        AFE47   = 0x10
    };
    Q_DECLARE_FLAGS(BoardTypes, BoardType);

    Board_Algorithm(BoardTypes types, int32_t mainParamAddr, int32_t mainParamIdx = 0, bool useMainParamAsSwitcher = true);

protected:
	virtual bool checkForAvailable() const override;

private:
    BoardTypes m_algBoardTypes;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(Board_Algorithm::BoardTypes);
