#include "../../../epch.h"
#include "WorldPackets.h"

namespace Skeleton {

void WorldPackets::SendMapRegion(StreamBuffer& outStream, ISAACCipher& encryptor, int32_t regionX, int32_t regionY, int32_t localX, int32_t localY, int32_t z)
{
    outStream.WriteHeader(221, encryptor, VariableHeaderSize::SHORT);
    outStream.WriteShort(regionY, ValueType::A);

    for (int xCalc = (regionX - 6) / 8; xCalc <= (regionX + 6) / 8; xCalc++) {
        for (int yCalc = (regionY - 6) / 8; yCalc <= (regionY + 6) / 8; yCalc++) {
            outStream.WriteInt(0, ValueType::STANDARD, ByteOrder::MIDDLE);
            outStream.WriteInt(0, ValueType::STANDARD, ByteOrder::MIDDLE);
            outStream.WriteInt(0, ValueType::STANDARD, ByteOrder::MIDDLE);
            outStream.WriteInt(0, ValueType::STANDARD, ByteOrder::MIDDLE);
        }
    }

    outStream.WriteShort(regionX, ValueType::STANDARD, ByteOrder::LITTLE);
    outStream.WriteShort(localX);
    outStream.WriteByte(z);
    outStream.WriteShort(localY);
    outStream.FinishVariableHeader();
}

void WorldPackets::SendArea(StreamBuffer& outStream, ISAACCipher& encryptor, int32_t x, int32_t y, int32_t regionX, int32_t regionY)
{
    outStream.WriteHeader(132, encryptor);

    int32_t localY = y - ((regionY - 6) * 8);
    int32_t localX = x - ((regionX - 6) * 8);

    outStream.WriteByte(localY);
    outStream.WriteByte(localX);
}

void WorldPackets::SendGroundItem(StreamBuffer& outStream, ISAACCipher& encryptor, int32_t itemId, int32_t count)
{
    outStream.WriteHeader(112, encryptor);
    outStream.WriteShort(itemId);
    outStream.WriteShort(count, ValueType::STANDARD, ByteOrder::LITTLE);
    outStream.WriteByte(0, ValueType::S);
}

void WorldPackets::SendRemoveGroundItem(StreamBuffer& outStream, ISAACCipher& encryptor, int32_t itemId)
{
    outStream.WriteHeader(39, encryptor);
    outStream.WriteShort(itemId, ValueType::A);
}

void WorldPackets::SendObject(StreamBuffer& outStream, ISAACCipher& encryptor, int32_t objectId, int32_t objectType, int32_t face)
{
    outStream.WriteHeader(17, encryptor);
    outStream.WriteByte(0, ValueType::A);
    outStream.WriteShort(objectId, ValueType::STANDARD, ByteOrder::LITTLE);
    outStream.WriteByte((objectType << 2) + (face & 3), ValueType::A);
}

void WorldPackets::RemoveObject(StreamBuffer& outStream, ISAACCipher& encryptor, int32_t objectType, int32_t face)
{
    outStream.WriteHeader(16, encryptor);
    outStream.WriteByte((objectType << 2) + (face & 3), ValueType::A);
    outStream.WriteByte(0, ValueType::A);
}

void WorldPackets::SendUpdateItems(StreamBuffer& outStream, ISAACCipher& encryptor, int32_t interfaceId, int32_t childId, int32_t type, const std::vector<std::pair<int32_t, int32_t>>& items)
{
    LOG_INFO("[PACKET] SendUpdateItems: opcode=92, interface={}, child={}, type={}, itemCount={}",
             interfaceId, childId, type, items.size());
    outStream.WriteHeader(92, encryptor, VariableHeaderSize::SHORT);
    outStream.WriteInt((interfaceId << 16) | childId);
    outStream.WriteShort(type);
    outStream.WriteShort(items.size());

    for (const auto& item : items) {
        int32_t itemId = item.first;
        int32_t count = item.second;

        if (itemId > 0) {
            if (count > 254) {
                outStream.WriteByte(255, ValueType::C);
                outStream.WriteInt(count);
            } else {
                outStream.WriteByte(count, ValueType::C);
            }
            outStream.WriteShort(itemId + 1, ValueType::STANDARD, ByteOrder::LITTLE);
        } else {
            outStream.WriteByte(0, ValueType::C);
            outStream.WriteShort(0, ValueType::STANDARD, ByteOrder::LITTLE);
        }
    }

    outStream.FinishVariableHeader();
}

void WorldPackets::SendConfig(StreamBuffer& outStream, ISAACCipher& encryptor, int32_t id, int32_t value)
{
    if (value < 128 && value > -128) {
        LOG_INFO("[PACKET] SendConfig: opcode=245, id={}, value={}", id, value);
        outStream.WriteHeader(245, encryptor);
        outStream.WriteShort(id, ValueType::A);
        outStream.WriteByte(value);
    } else {
        LOG_INFO("[PACKET] SendConfig: opcode=37, id={}, value={}", id, value);
        outStream.WriteHeader(37, encryptor);
        outStream.WriteShort(id, ValueType::A);
        outStream.WriteInt(value, ValueType::STANDARD, ByteOrder::LITTLE);
    }
}

}
