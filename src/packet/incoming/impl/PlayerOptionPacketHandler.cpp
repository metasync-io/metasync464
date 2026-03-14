#include "PlayerOptionPacketHandler.h"

namespace Skeleton {

void PlayerOptionPacketHandler::handle(std::shared_ptr<Player> player, StreamBuffer& inStream, int32_t opcode, int32_t length) {
    switch (opcode) {
        case 128:
            option1(player, inStream);
            break;
        case 37:
            option2(player, inStream);
            break;
        case 227:
            option3(player, inStream);
            break;
    }
}

void PlayerOptionPacketHandler::option1(std::shared_ptr<Player> player, StreamBuffer& inStream) {
    int32_t id = inStream.ReadShort() & 0xFFFF;
    if (id < 0 || id >= 2048) {
        return;
    }
}

void PlayerOptionPacketHandler::option2(std::shared_ptr<Player> player, StreamBuffer& inStream) {
    int32_t id = inStream.ReadShort() & 0xFFFF;
    if (id < 0 || id >= 2048) {
        return;
    }
}

void PlayerOptionPacketHandler::option3(std::shared_ptr<Player> player, StreamBuffer& inStream) {
    int32_t id = inStream.ReadShort(ValueType::A, ByteOrder::LITTLE) & 0xFFFF;
    if (id < 0 || id >= 2048) {
        return;
    }
}

}
