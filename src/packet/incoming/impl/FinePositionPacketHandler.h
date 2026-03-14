#pragma once
#include "../PacketHandler.h"

namespace Skeleton {

class FinePositionPacketHandler : public PacketHandler {
public:
    void handle(std::shared_ptr<Player> player, StreamBuffer& inStream, int32_t opcode, int32_t length) override;
};

}
