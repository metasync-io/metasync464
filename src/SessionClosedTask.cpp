#include "epch.h"
#include "SessionClosedTask.h"
#include "Client.h"

namespace Skeleton {

SessionClosedTask::SessionClosedTask(std::shared_ptr<Client> client)
    : m_Client(client)
{
}

void SessionClosedTask::execute(GameEngine& engine) {
    m_Client->Disconnect();
    LOG_INFO("Session closed");
}

}
