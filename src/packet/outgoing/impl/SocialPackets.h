#pragma once
#include "../../../StreamBuffer.h"
#include "../../../ISAACCipher.h"
#include <vector>
#include <cstdint>

namespace Skeleton {

class SocialPackets {
public:
    static void SendFriendListStatus(StreamBuffer& outStream, ISAACCipher& encryptor, int32_t status);  // Packet 152 - Set friends list network status
    static void SendPrivateMessage(StreamBuffer& outStream, ISAACCipher& encryptor, int64_t name, const std::vector<uint8_t>& message);  // Packet 23 - Send private message
    static void SendAddFriend(StreamBuffer& outStream, ISAACCipher& encryptor, int64_t name, int32_t worldId);  // Packet 100 - Add friend to list
    static void SendIgnoreList(StreamBuffer& outStream, ISAACCipher& encryptor, const std::vector<int64_t>& ignoreList);  // Packet 75 - Send ignore list
};

}
