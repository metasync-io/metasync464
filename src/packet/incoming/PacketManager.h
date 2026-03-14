#pragma once
#include "../../epch.h"
#include "PacketHandler.h"

namespace Skeleton {

class PacketManager {
public:
    static PacketManager& getInstance();

    void bind(int32_t opcode, std::shared_ptr<PacketHandler> handler);

    std::shared_ptr<PacketHandler> getHandler(int32_t opcode);

private:
    PacketManager() = default;
    std::unordered_map<int32_t, std::shared_ptr<PacketHandler>> m_Handlers;
};

}
