#pragma once
#include "Task.h"
#include <memory>
#include <vector>

namespace Skeleton {

class Player;

class PlayerUpdateTask : public Task {
public:
    explicit PlayerUpdateTask(std::vector<std::shared_ptr<Player>> players);
    void execute(GameEngine& engine) override;

private:
    std::vector<std::shared_ptr<Player>> m_Players;
};

}
