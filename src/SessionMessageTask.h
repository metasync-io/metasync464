#pragma once
#include "Task.h"
#include "StreamBuffer.h"
#include <memory>

namespace Skeleton {

class Client;

class SessionMessageTask : public Task {
public:
    SessionMessageTask(std::shared_ptr<Client> client, StreamBuffer buffer);
    void execute(GameEngine& engine) override;

private:
    std::shared_ptr<Client> m_Client;
    StreamBuffer m_Buffer;
};

}
