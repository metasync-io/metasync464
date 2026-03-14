#include "epch.h"
#include "PlayerTickTask.h"
#include "Player.h"

namespace Skeleton {

PlayerTickTask::PlayerTickTask(std::vector<std::shared_ptr<Player>> players)
    : m_Players(std::move(players))
{
}

void PlayerTickTask::execute(GameEngine& engine) {
    for (auto& player : m_Players) {
        if (player) {
            player->Process();
        }
    }
}

}
