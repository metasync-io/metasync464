#pragma once
#include "../../epch.h"
#include "../../StreamBuffer.h"

namespace Skeleton {

class Player;

class PacketHandler {
public:
    virtual ~PacketHandler() = default;
    virtual void handle(std::shared_ptr<Player> player, StreamBuffer& inStream, int32_t opcode, int32_t length) = 0;
};

}
