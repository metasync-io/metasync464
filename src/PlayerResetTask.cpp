#include "epch.h"
#include "PlayerResetTask.h"
#include "Player.h"

namespace Skeleton {

PlayerResetTask::PlayerResetTask(std::vector<std::shared_ptr<Player>> players)
    : m_Players(std::move(players))
{
}

void PlayerResetTask::execute(GameEngine& engine) {
    for (auto& player : m_Players) {
        if (player) {
            player->Reset();
        }
    }
}

}
