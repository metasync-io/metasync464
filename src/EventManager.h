#pragma once
#include "epch.h"
#include "Event.h"

namespace Skeleton {

class GameEngine;

class EventManager {
public:
    explicit EventManager(GameEngine& engine);

    void submit(std::shared_ptr<Event> event);

    void process();

private:
    GameEngine& m_Engine;
    std::list<std::pair<std::shared_ptr<Event>, int32_t>> m_Events;
};

}
