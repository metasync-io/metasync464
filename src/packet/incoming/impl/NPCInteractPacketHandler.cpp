#include "NPCInteractPacketHandler.h"

namespace Skeleton {

void NPCInteractPacketHandler::handle(std::shared_ptr<Player> player, StreamBuffer& inStream, int32_t opcode, int32_t length) {
    switch (opcode) {
        case 129:
            handleAttackOption(player, inStream);
            break;
        case 156:
            break;
        case 19:
            handleTradeOption(player, inStream);
            break;
        case 72:
            handleExamineOption(player, inStream);
            break;
    }
}

void NPCInteractPacketHandler::handleAttackOption(std::shared_ptr<Player> player, StreamBuffer& inStream) {
    int32_t npcIndex = inStream.ReadShort(ByteOrder::LITTLE) & 0xFFFF;
    if (npcIndex < 0 || npcIndex > 32768) {
        return;
    }
}

void NPCInteractPacketHandler::handleExamineOption(std::shared_ptr<Player> player, StreamBuffer& inStream) {
    int32_t id = inStream.ReadShort();
}

void NPCInteractPacketHandler::handleTradeOption(std::shared_ptr<Player> player, StreamBuffer& inStream) {
    int32_t npcIndex = inStream.ReadShort(ValueType::A, ByteOrder::LITTLE);
    if (npcIndex < 0 || npcIndex > 32768) {
        return;
    }
}

}
