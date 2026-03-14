#include "WieldPacketHandler.h"
#include "../../../Player.h"

namespace Skeleton {

void WieldPacketHandler::handle(std::shared_ptr<Player> player, StreamBuffer& inStream, int32_t opcode, int32_t length) {
    switch (opcode) {
        case 215:
            handleItemWield(player, inStream);
            break;
        case 177:
            handleItemUnequip(player, inStream);
            break;
    }
}

void WieldPacketHandler::handleItemWield(std::shared_ptr<Player> player, StreamBuffer& inStream) {
    int32_t slot = inStream.ReadShort(ByteOrder::LITTLE);
    int32_t interfaceId = inStream.ReadInt(ByteOrder::LITTLE) >> 16;
    int32_t id = inStream.ReadShort(ByteOrder::LITTLE);
    
    if (interfaceId != 149) return;
    
    if (slot >= 0 && slot < 28) {
    }
}

void WieldPacketHandler::handleItemUnequip(std::shared_ptr<Player> player, StreamBuffer& inStream) {
    int32_t id = inStream.ReadShort();
    int32_t interfaceHash = inStream.ReadInt();
    int32_t slot = inStream.ReadShort();
    int32_t interfaceId = interfaceHash >> 16;
    int32_t child = interfaceHash & 0xff;
    
    if (slot >= 0 && slot <= 10) {
    }
}

}
