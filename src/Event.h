#pragma once
#include "epch.h"

namespace Skeleton {

class Event {
public:
    virtual ~Event() = default;

    virtual void execute() = 0;

    virtual int32_t getDelay() const = 0;

    bool isRunning() const { return m_Running; }
    void stop() { m_Running = false; }

protected:
    bool m_Running = true;
};

}
