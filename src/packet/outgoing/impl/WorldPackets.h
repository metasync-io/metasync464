#pragma once
#include "../../../StreamBuffer.h"
#include "../../../ISAACCipher.h"

namespace Skeleton {

class WorldPackets {
public:
    static void SendMapRegion(StreamBuffer& outStream, ISAACCipher& encryptor, int32_t regionX, int32_t regionY, int32_t localX, int32_t localY, int32_t z);  // Packet 221 - Load map region
    static void SendArea(StreamBuffer& outStream, ISAACCipher& encryptor, int32_t x, int32_t y, int32_t regionX, int32_t regionY);  // Packet 132 - Set current area for localized updates
    static void SendGroundItem(StreamBuffer& outStream, ISAACCipher& encryptor, int32_t itemId, int32_t count);  // Packet 112 - Spawn ground item
    static void SendRemoveGroundItem(StreamBuffer& outStream, ISAACCipher& encryptor, int32_t itemId);  // Packet 39 - Remove ground item
    static void SendObject(StreamBuffer& outStream, ISAACCipher& encryptor, int32_t objectId, int32_t objectType, int32_t face);  // Packet 17 - Spawn game object
    static void RemoveObject(StreamBuffer& outStream, ISAACCipher& encryptor, int32_t objectType, int32_t face);  // Packet 16 - Remove game object
    static void SendUpdateItems(StreamBuffer& outStream, ISAACCipher& encryptor, int32_t interfaceId, int32_t childId, int32_t type, const std::vector<std::pair<int32_t, int32_t>>& items);  // Packet 92 - Update inventory/container items
    static void SendConfig(StreamBuffer& outStream, ISAACCipher& encryptor, int32_t id, int32_t value);  // Packet 245/37 - Set client config value
};

}
