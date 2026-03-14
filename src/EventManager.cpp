#include "epch.h"
#include "EventManager.h"
#include "GameEngine.h"

namespace Skeleton {

EventManager::EventManager(GameEngine& engine)
    : m_Engine(engine)
{
}

void EventManager::submit(std::shared_ptr<Event> event) {
    m_Events.push_back({event, event->getDelay()});
}

void EventManager::process() {
    for (auto it = m_Events.begin(); it != m_Events.end(); ) {
        auto& [event, ticksRemaining] = *it;

        if (ticksRemaining > 0) {
            ticksRemaining--;
            ++it;
            continue;
        }

        try {
            event->execute();
        } catch (const std::exception& e) {
            LOG_ERROR("Exception in event: {}", e.what());
            event->stop();
        }

        if (!event->isRunning()) {
            it = m_Events.erase(it);
        } else {
            ticksRemaining = event->getDelay();
            ++it;
        }
    }
}

}
