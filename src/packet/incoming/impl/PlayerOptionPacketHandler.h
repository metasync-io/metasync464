#pragma once
#include "../PacketHandler.h"

namespace Skeleton {

class PlayerOptionPacketHandler : public PacketHandler {
public:
    void handle(std::shared_ptr<Player> player, StreamBuffer& inStream, int32_t opcode, int32_t length) override;

private:
    void option1(std::shared_ptr<Player> player, StreamBuffer& inStream);
    void option2(std::shared_ptr<Player> player, StreamBuffer& inStream);
    void option3(std::shared_ptr<Player> player, StreamBuffer& inStream);
};

}
