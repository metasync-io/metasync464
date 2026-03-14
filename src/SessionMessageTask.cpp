#include "epch.h"
#include "SessionMessageTask.h"
#include "Client.h"

namespace Skeleton {

SessionMessageTask::SessionMessageTask(std::shared_ptr<Client> client, StreamBuffer buffer)
    : m_Client(client), m_Buffer(std::move(buffer))
{
}

void SessionMessageTask::execute(GameEngine& engine) {
    m_Client->HandleIncomingDataTask(m_Buffer);
}

}
