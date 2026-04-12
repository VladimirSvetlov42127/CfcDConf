#include "dout_physical_signal.h"

namespace {

constexpr const char* PROPERTY_DISABLE_CLONING = "disable_cloning";

} // namespace

DoutPhysicalSignal::DoutPhysicalSignal(const Config &config)
    : DoutSignal{config}
{

}

bool DoutPhysicalSignal::isCloningEnabled() const
{
    return !properties().get(PROPERTY_DISABLE_CLONING).toUInt();
}
