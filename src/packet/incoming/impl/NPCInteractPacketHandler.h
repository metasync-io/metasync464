#pragma once
#include "../PacketHandler.h"

namespace Skeleton {

class NPCInteractPacketHandler : public PacketHandler {
public:
    void handle(std::shared_ptr<Player> player, StreamBuffer& inStream, int32_t opcode, int32_t length) override;

private:
    void handleAttackOption(std::shared_ptr<Player> player, StreamBuffer& inStream);
    void handleExamineOption(std::shared_ptr<Player> player, StreamBuffer& inStream);
    void handleTradeOption(std::shared_ptr<Player> player, StreamBuffer& inStream);
};

}
