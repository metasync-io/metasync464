#pragma once

namespace Skeleton {

class GameEngine;

class Task {
public:
    virtual ~Task() = default;
    virtual void execute(GameEngine& engine) = 0;
};

}
