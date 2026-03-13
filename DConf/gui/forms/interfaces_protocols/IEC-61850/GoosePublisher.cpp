#include "GoosePublisher.h"

#include <qdebug.h>

namespace {

} // namespace

GooseSubscriberEntryCfg::GooseSubscriberEntryCfg(uint32_t value)
{
	memcpy(this, &value, sizeof(GooseSubscriberEntryCfg));
}

uint32_t GooseSubscriberEntryCfg::toUint() const
{
	uint32_t v;
	memcpy(&v, this, sizeof(GooseSubscriberEntryCfg)); 
	return v;
}
