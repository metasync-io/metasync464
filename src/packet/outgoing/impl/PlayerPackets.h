#pragma once
#include "../../../StreamBuffer.h"
#include "../../../ISAACCipher.h"
#include <string>

namespace Skeleton {

class PlayerPackets {
public:
    static void SendSkill(StreamBuffer& outStream, ISAACCipher& encryptor, int32_t skill, int32_t level, int32_t experience);  // Packet 190 - Update player skill level/exp
    static void SendPlayerMessage(StreamBuffer& outStream, ISAACCipher& encryptor, const std::string& message);  // Packet 108 - Send message to chatbox
    static void SendInteractionOption(StreamBuffer& outStream, ISAACCipher& encryptor, const std::string& option, int32_t slot, bool top);  // Packet 72 - Add player interaction option
};

}
