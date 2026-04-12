#ifndef AINVIRTUALARCHIVESIGNAL_H
#define AINVIRTUALARCHIVESIGNAL_H

#include "service_manager/signals/ain_signal.h"

class AinVirtualArchiveSignal : public AinSignal
{
public:
    AinVirtualArchiveSignal(const Config &config);
    virtual ~AinVirtualArchiveSignal() = default;

    Signal::Subtype subtype() const override { return Signal::Subtype::ArchiveVirtual; }
};

#endif // AINVIRTUALARCHIVESIGNAL_H
