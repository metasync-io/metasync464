#include "../../../epch.h"
#include "PlayerPackets.h"

namespace Skeleton {

void PlayerPackets::SendSkill(StreamBuffer& outStream, ISAACCipher& encryptor, int32_t skill, int32_t level, int32_t experience)
{
    LOG_INFO("[PACKET] SendSkill: opcode=190, skill={}, level={}, exp={}",
             skill, level, experience);
    outStream.WriteHeader(190, encryptor);
    outStream.WriteByte(skill, ValueType::S);
    outStream.WriteInt(experience, ValueType::STANDARD, ByteOrder::LITTLE);
    outStream.WriteByte(level);
}

void PlayerPackets::SendPlayerMessage(StreamBuffer& outStream, ISAACCipher& encryptor, const std::string& message)
{
    LOG_INFO("[PACKET] SendPlayerMessage: opcode=108, message=\"{}\"", message);
    outStream.WriteHeader(108, encryptor, VariableHeaderSize::BYTE);
    outStream.WriteString(message);
    outStream.FinishVariableHeader();
}

void PlayerPackets::SendInteractionOption(StreamBuffer& outStream, ISAACCipher& encryptor, const std::string& option, int32_t slot, bool top)
{
    LOG_INFO("[PACKET] SendInteractionOption: opcode=72, option=\"{}\", slot={}, top={}",
             option, slot, top);
    outStream.WriteHeader(72, encryptor, VariableHeaderSize::BYTE);
    outStream.WriteString(option);
    outStream.WriteByte(slot, ValueType::S);
    outStream.WriteByte(top ? 0 : 1, ValueType::C);
    outStream.FinishVariableHeader();
}

}
