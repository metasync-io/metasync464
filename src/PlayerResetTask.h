#pragma once
#include "Task.h"
#include <memory>
#include <vector>

namespace Skeleton {

class Player;

class PlayerResetTask : public Task {
public:
    explicit PlayerResetTask(std::vector<std::shared_ptr<Player>> players);
    void execute(GameEngine& engine) override;

private:
    std::vector<std::shared_ptr<Player>> m_Players;
};

}
