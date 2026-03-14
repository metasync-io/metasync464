#include "../../../epch.h"
#include "SystemPackets.h"

namespace Skeleton {

void SystemPackets::SendLogout(StreamBuffer& outStream, ISAACCipher& encryptor)
{
    LOG_INFO("[PACKET] SendLogout: opcode=167");
    outStream.WriteHeader(167, encryptor);
}

void SystemPackets::SendSystemUpdate(StreamBuffer& outStream, ISAACCipher& encryptor, int32_t time)
{
    LOG_INFO("[PACKET] SendSystemUpdate: opcode=30, time={}", time);
    outStream.WriteHeader(30, encryptor);
    outStream.WriteShort(time);
}

}
