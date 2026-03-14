#pragma once
#include "Task.h"
#include <memory>
#include <vector>

namespace Skeleton {

class Player;

class PlayerTickTask : public Task {
public:
    explicit PlayerTickTask(std::vector<std::shared_ptr<Player>> players);
    void execute(GameEngine& engine) override;

private:
    std::vector<std::shared_ptr<Player>> m_Players;
};

}
