#pragma once

#include "events.h"   // must define EventType

class IEventSink {
public:
    virtual ~IEventSink() = default;

    virtual void postEvent(EventType type) = 0;
};