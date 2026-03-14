#include "../../../epch.h"
#include "SocialPackets.h"

namespace Skeleton {

void SocialPackets::SendFriendListStatus(StreamBuffer& outStream, ISAACCipher& encryptor, int32_t status)
{
    LOG_INFO("[PACKET] SendFriendListStatus: opcode=152, status={}", status);
    outStream.WriteHeader(152, encryptor);
    outStream.WriteByte(status);
}

void SocialPackets::SendPrivateMessage(StreamBuffer& outStream, ISAACCipher& encryptor, int64_t name, const std::vector<uint8_t>& message)
{
    LOG_INFO("[PACKET] SendPrivateMessage: opcode=23, name={}, messageSize={}", name, message.size());
    outStream.WriteHeader(23, encryptor, VariableHeaderSize::SHORT);
    outStream.WriteLong(name);
    for (uint8_t byte : message) {
        outStream.WriteByte(byte);
    }
    outStream.FinishVariableHeader();
}

void SocialPackets::SendAddFriend(StreamBuffer& outStream, ISAACCipher& encryptor, int64_t name, int32_t worldId)
{
    LOG_INFO("[PACKET] SendAddFriend: opcode=100, name={}, world={}", name, worldId);
    outStream.WriteHeader(100, encryptor);
    outStream.WriteLong(name);
    outStream.WriteShort(worldId);
}

void SocialPackets::SendIgnoreList(StreamBuffer& outStream, ISAACCipher& encryptor, const std::vector<int64_t>& ignoreList)
{
    LOG_INFO("[PACKET] SendIgnoreList: opcode=75, count={}", ignoreList.size());
    outStream.WriteHeader(75, encryptor, VariableHeaderSize::SHORT);
    for (int64_t name : ignoreList) {
        outStream.WriteLong(name);
    }
    outStream.FinishVariableHeader();
}

}
