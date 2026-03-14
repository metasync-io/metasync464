#include "ItemOptionPacketHandler.h"

namespace Skeleton {

void ItemOptionPacketHandler::handle(std::shared_ptr<Player> player, StreamBuffer& inStream, int32_t opcode, int32_t length) {
    switch (opcode) {
        case 145:
            handleItemOption1(player, inStream);
            break;
        case 117:
            handleItemOption2(player, inStream);
            break;
        case 43:
            handleItemOption3(player, inStream);
            break;
        case 129:
            handleItemOption4(player, inStream);
            break;
        case 135:
            handleItemOption5(player, inStream);
            break;
        case 101:
            handleItemOptionClick1(player, inStream);
            break;
        case 247:
            handleDropItem(player, inStream);
            break;
        case 216:
            handlePickupGroundItem(player, inStream);
            break;
    }
}

void ItemOptionPacketHandler::handleItemOption1(std::shared_ptr<Player> player, StreamBuffer& inStream) {
    int32_t interfaceId = inStream.ReadShort(ValueType::A, ByteOrder::BIG) & 0xFFFF;
    int32_t slot = inStream.ReadShort(ValueType::A, ByteOrder::BIG) & 0xFFFF;
    int32_t id = inStream.ReadShort(ValueType::A, ByteOrder::BIG) & 0xFFFF;
}

void ItemOptionPacketHandler::handleItemOption2(std::shared_ptr<Player> player, StreamBuffer& inStream) {
    int32_t interfaceId = inStream.ReadShort(ValueType::A, ByteOrder::LITTLE) & 0xFFFF;
    int32_t id = inStream.ReadShort(ValueType::A, ByteOrder::LITTLE) & 0xFFFF;
    int32_t slot = inStream.ReadShort(ByteOrder::LITTLE) & 0xFFFF;
}

void ItemOptionPacketHandler::handleItemOption3(std::shared_ptr<Player> player, StreamBuffer& inStream) {
    int32_t interfaceId = inStream.ReadShort(ByteOrder::LITTLE) & 0xFFFF;
    int32_t id = inStream.ReadShort(ValueType::A, ByteOrder::BIG) & 0xFFFF;
    int32_t slot = inStream.ReadShort(ValueType::A, ByteOrder::BIG) & 0xFFFF;
}

void ItemOptionPacketHandler::handleItemOption4(std::shared_ptr<Player> player, StreamBuffer& inStream) {
    int32_t slot = inStream.ReadShort(ValueType::A, ByteOrder::BIG) & 0xFFFF;
    int32_t interfaceId = inStream.ReadShort() & 0xFFFF;
    int32_t id = inStream.ReadShort(ValueType::A, ByteOrder::BIG) & 0xFFFF;
}

void ItemOptionPacketHandler::handleItemOption5(std::shared_ptr<Player> player, StreamBuffer& inStream) {
    int32_t slot = inStream.ReadShort(ByteOrder::LITTLE) & 0xFFFF;
    int32_t interfaceId = inStream.ReadShort(ValueType::A, ByteOrder::BIG) & 0xFFFF;
    int32_t id = inStream.ReadShort(ByteOrder::LITTLE) & 0xFFFF;
}

void ItemOptionPacketHandler::handleItemOptionClick1(std::shared_ptr<Player> player, StreamBuffer& inStream) {
    int32_t interfaceId = inStream.ReadInt() >> 16;
    int32_t slot = inStream.ReadShort(ByteOrder::LITTLE);
    int32_t id = inStream.ReadShort(ByteOrder::LITTLE);
}

void ItemOptionPacketHandler::handleDropItem(std::shared_ptr<Player> player, StreamBuffer& inStream) {
    int32_t interfaceId = inStream.ReadInt();
    int32_t id = inStream.ReadShort(ValueType::A, ByteOrder::BIG);
    int32_t slot = inStream.ReadShort(ByteOrder::LITTLE);
}

void ItemOptionPacketHandler::handlePickupGroundItem(std::shared_ptr<Player> player, StreamBuffer& inStream) {
    int32_t id = inStream.ReadShort();
    int32_t yPos = inStream.ReadShort(ValueType::A, ByteOrder::BIG);
    int32_t xPos = inStream.ReadShort(ValueType::A, ByteOrder::LITTLE);
}

}
