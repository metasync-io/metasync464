#pragma once
#include "Event.h"

namespace Skeleton {

class UpdateEvent : public Event {
public:
    UpdateEvent();
    void execute() override;
    int32_t getDelay() const override { return 1; }
};

}
