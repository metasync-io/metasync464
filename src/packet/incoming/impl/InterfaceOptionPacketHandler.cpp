#include "InterfaceOptionPacketHandler.h"

namespace Skeleton {

void InterfaceOptionPacketHandler::handle(std::shared_ptr<Player> player, StreamBuffer& inStream, int32_t opcode, int32_t length) {
    switch (opcode) {
        case 177:
            handleClickOne(player, inStream);
            break;
        case 88:
            handleClickTwo(player, inStream);
            break;
        case 159:
            handleClickThree(player, inStream);
            break;
        case 86:
            handleClickFour(player, inStream);
            break;
    }
}

void InterfaceOptionPacketHandler::handleClickOne(std::shared_ptr<Player> player, StreamBuffer& inStream) {
    int32_t itemId = inStream.ReadShort();
    int32_t interfaceSet = inStream.ReadInt();
    int32_t slot = inStream.ReadShort();
    int32_t interfaceId = interfaceSet >> 16;
    int32_t child = interfaceSet & 0xffff;
    
    switch (interfaceId) {
        case 12:
            if (slot < 0 || slot > 400) break;
            break;
        case 15:
            if (slot < 0 || slot > 27) break;
            break;
        case 300:
            break;
        case 301:
            break;
        case 387:
        case 465:
            if (slot < 0 || slot > 13) break;
            break;
    }
}

void InterfaceOptionPacketHandler::handleClickTwo(std::shared_ptr<Player> player, StreamBuffer& inStream) {
    int32_t interfaceSet = inStream.ReadInt(ByteOrder::LITTLE);
    int32_t interfaceId = interfaceSet >> 16;
    int32_t child = interfaceSet & 0xffff;
    int32_t itemId = inStream.ReadShort(ByteOrder::LITTLE) & 0xffff;
    int32_t slot = inStream.ReadShort(ValueType::A, ByteOrder::LITTLE) & 0xffff;
    
    switch (interfaceId) {
        case 12:
            if (slot < 0 || slot > 400) break;
            break;
        case 15:
            if (slot < 0 || slot > 27) break;
            break;
        case 300:
            break;
    }
}

void InterfaceOptionPacketHandler::handleClickThree(std::shared_ptr<Player> player, StreamBuffer& inStream) {
    int32_t interfaceSet = inStream.ReadInt(ByteOrder::LITTLE);
    int32_t interfaceId = interfaceSet >> 16;
    int32_t child = interfaceSet & 0xffff;
    int32_t slot = inStream.ReadShort(ByteOrder::LITTLE) & 0xffff;
    int32_t itemId = inStream.ReadShort(ByteOrder::LITTLE) & 0xffff;
    
    switch (interfaceId) {
        case 12:
            if (slot < 0 || slot > 400) break;
            break;
        case 15:
            if (slot < 0 || slot > 27) break;
            break;
        case 300:
            break;
    }
}

void InterfaceOptionPacketHandler::handleClickFour(std::shared_ptr<Player> player, StreamBuffer& inStream) {
    int32_t slot = inStream.ReadShort(ByteOrder::LITTLE) & 0xFFFF;
    int32_t itemId = inStream.ReadShort() & 0xFFFF;
    int32_t interfaceSet = inStream.ReadInt();
    int32_t interfaceId = interfaceSet >> 16;
    int32_t child = interfaceSet & 0xffff;
    
    switch (interfaceId) {
        case 12:
            if (slot < 0 || slot > 400) break;
            break;
        case 15:
            if (slot < 0 || slot > 27) break;
            break;
        case 300:
            break;
    }
}

}
