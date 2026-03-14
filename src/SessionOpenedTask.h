#pragma once
#include "Task.h"
#include <memory>

namespace Skeleton {

class Client;

class SessionOpenedTask : public Task {
public:
    explicit SessionOpenedTask(std::shared_ptr<Client> client);
    void execute(GameEngine& engine) override;

private:
    std::shared_ptr<Client> m_Client;
};

}
