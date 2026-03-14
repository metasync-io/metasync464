#pragma once
#include "../PacketHandler.h"

namespace Skeleton {

class ObjectOptionPacketHandler : public PacketHandler {
public:
    void handle(std::shared_ptr<Player> player, StreamBuffer& inStream, int32_t opcode, int32_t length) override;

private:
    void handleOption1(std::shared_ptr<Player> player, StreamBuffer& inStream);
    void handleOption2(std::shared_ptr<Player> player, StreamBuffer& inStream);
};

}
