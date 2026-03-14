#pragma once
#include "../../../StreamBuffer.h"
#include "../../../ISAACCipher.h"

namespace Skeleton {

class SystemPackets {
public:
    static void SendLogout(StreamBuffer& outStream, ISAACCipher& encryptor);  // Packet 167 - Logout packet
    static void SendSystemUpdate(StreamBuffer& outStream, ISAACCipher& encryptor, int32_t time);  // Packet 30 - System update countdown timer
};

}
