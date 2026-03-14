#pragma once
#include "../PacketHandler.h"

namespace Skeleton {

class ItemOptionPacketHandler : public PacketHandler {
public:
    void handle(std::shared_ptr<Player> player, StreamBuffer& inStream, int32_t opcode, int32_t length) override;

private:
    void handleItemOption1(std::shared_ptr<Player> player, StreamBuffer& inStream);
    void handleItemOption2(std::shared_ptr<Player> player, StreamBuffer& inStream);
    void handleItemOption3(std::shared_ptr<Player> player, StreamBuffer& inStream);
    void handleItemOption4(std::shared_ptr<Player> player, StreamBuffer& inStream);
    void handleItemOption5(std::shared_ptr<Player> player, StreamBuffer& inStream);
    void handleItemOptionClick1(std::shared_ptr<Player> player, StreamBuffer& inStream);
    void handleDropItem(std::shared_ptr<Player> player, StreamBuffer& inStream);
    void handlePickupGroundItem(std::shared_ptr<Player> player, StreamBuffer& inStream);
};

}
