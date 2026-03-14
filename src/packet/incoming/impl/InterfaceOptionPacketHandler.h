#pragma once
#include "../PacketHandler.h"

namespace Skeleton {

class InterfaceOptionPacketHandler : public PacketHandler {
public:
    void handle(std::shared_ptr<Player> player, StreamBuffer& inStream, int32_t opcode, int32_t length) override;

private:
    void handleClickOne(std::shared_ptr<Player> player, StreamBuffer& inStream);
    void handleClickTwo(std::shared_ptr<Player> player, StreamBuffer& inStream);
    void handleClickThree(std::shared_ptr<Player> player, StreamBuffer& inStream);
    void handleClickFour(std::shared_ptr<Player> player, StreamBuffer& inStream);
};

}
