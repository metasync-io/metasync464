#pragma once
#include "../PacketHandler.h"

namespace Skeleton {

class Turn180PacketHandler : public PacketHandler {
public:
    void handle(std::shared_ptr<Player> player, StreamBuffer& inStream, int32_t opcode, int32_t length) override;
};

}
