#include "epch.h"
#include "SessionOpenedTask.h"
#include "Client.h"

namespace Skeleton {

SessionOpenedTask::SessionOpenedTask(std::shared_ptr<Client> client)
    : m_Client(client)
{
}

void SessionOpenedTask::execute(GameEngine& engine) {
    m_Client->Start();
    LOG_INFO("Session opened");
}

}
