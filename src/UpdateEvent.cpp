#include "epch.h"
#include "UpdateEvent.h"
#include "World.h"
#include "GameEngine.h"
#include "PlayerTickTask.h"
#include "PlayerUpdateTask.h"
#include "PlayerResetTask.h"

namespace Skeleton {

UpdateEvent::UpdateEvent() {
}

void UpdateEvent::execute() {
    // NOTE: This event is processed BY World::Process() via EventManager::process()
    // We should NOT call World::Process() here as that would cause infinite recursion!
    //
    // In the Java version, UpdateEvent does the actual player tick/update/reset work.
    // But in this C++ version, World::Process() already does that work before calling
    // EventManager::process(). So this event is essentially a no-op placeholder.
    //
    // If we need UpdateEvent to do work, we should refactor so that:
    // - GameEngine::runPulse() only calls EventManager::process()
    // - UpdateEvent::execute() does the actual player tick/update/reset
    //
    // For now, this is intentionally empty to prevent the stack overflow crash.
    
    // LOG_DEBUG("UpdateEvent::execute() - tick");
}

}
