#include "ObjectOptionPacketHandler.h"

namespace Skeleton {

void ObjectOptionPacketHandler::handle(std::shared_ptr<Player> player, StreamBuffer& inStream, int32_t opcode, int32_t length) {
    switch (opcode) {
        case 44:
            handleOption1(player, inStream);
            break;
        case 252:
            handleOption2(player, inStream);
            break;
    }
}

void ObjectOptionPacketHandler::handleOption1(std::shared_ptr<Player> player, StreamBuffer& inStream) {
    int32_t id = inStream.ReadShort(ByteOrder::LITTLE) & 0xFFFF;
    int32_t x = inStream.ReadShort() & 0xFFFF;
    int32_t y = inStream.ReadShort(ByteOrder::LITTLE) & 0xFFFF;
}

void ObjectOptionPacketHandler::handleOption2(std::shared_ptr<Player> player, StreamBuffer& inStream) {
    int32_t id = inStream.ReadShort(ByteOrder::LITTLE) & 0xFFFF;
    int32_t x = inStream.ReadShort() & 0xFFFF;
    int32_t y = inStream.ReadShort(ByteOrder::LITTLE) & 0xFFFF;
}

}
