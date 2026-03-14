#include "SwitchItemPacketHandler.h"

namespace Skeleton {

void SwitchItemPacketHandler::handle(std::shared_ptr<Player> player, StreamBuffer& inStream, int32_t opcode, int32_t length) {
    int32_t interfaceId = inStream.ReadShort(ValueType::A, ByteOrder::LITTLE) & 0xFFFF;
    inStream.ReadByte(ValueType::C);
    int32_t fromSlot = inStream.ReadShort(ValueType::A, ByteOrder::LITTLE) & 0xFFFF;
    int32_t toSlot = inStream.ReadShort(ByteOrder::LITTLE) & 0xFFFF;
    
    switch (interfaceId) {
        case 149:
            if (fromSlot >= 0 && fromSlot < 28 && toSlot >= 0 && toSlot < 28 && toSlot != fromSlot) {
            }
            break;
        case 516:
            if (fromSlot >= 0 && fromSlot < 400 && toSlot >= 0 && toSlot < 400 && toSlot != fromSlot) {
            }
            break;
    }
}

}
