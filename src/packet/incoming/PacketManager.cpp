#include "../../epch.h"
#include "PacketManager.h"

namespace Skeleton {

PacketManager& PacketManager::getInstance() {
    static PacketManager instance;
    return instance;
}

void PacketManager::bind(int32_t opcode, std::shared_ptr<PacketHandler> handler) {
    m_Handlers[opcode] = handler;
    LOG_INFO("Bound packet handler for opcode: {}", opcode);
}

std::shared_ptr<PacketHandler> PacketManager::getHandler(int32_t opcode) {
    auto it = m_Handlers.find(opcode);
    if (it != m_Handlers.end()) {
        return it->second;
    }
    return nullptr;
}

}
