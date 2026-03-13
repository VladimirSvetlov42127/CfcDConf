#include "BasePSCAlgorithm.h"

bool BasePSCAlgorithm::checkForAvailable() const
{
	if (DcController::P_SC == controller()->type())
		return true;

	return false;
}