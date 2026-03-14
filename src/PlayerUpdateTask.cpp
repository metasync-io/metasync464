#include "epch.h"
#include "PlayerUpdateTask.h"
#include "Player.h"
#include "PlayerUpdating.h"

namespace Skeleton {

PlayerUpdateTask::PlayerUpdateTask(std::vector<std::shared_ptr<Player>> players)
    : m_Players(std::move(players))
{
}

void PlayerUpdateTask::execute(GameEngine& engine) {
    PlayerUpdating updating;
    updating.Update(m_Players);
}

}
