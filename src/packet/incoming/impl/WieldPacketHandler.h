#pragma once
#include "../PacketHandler.h"

namespace Skeleton {

class WieldPacketHandler : public PacketHandler {
public:
    void handle(std::shared_ptr<Player> player, StreamBuffer& inStream, int32_t opcode, int32_t length) override;

private:
    void handleItemWield(std::shared_ptr<Player> player, StreamBuffer& inStream);
    void handleItemUnequip(std::shared_ptr<Player> player, StreamBuffer& inStream);
};

}
