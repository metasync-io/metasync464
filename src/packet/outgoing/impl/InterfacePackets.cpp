#include "../../../epch.h"
#include "InterfacePackets.h"

namespace Skeleton {

void InterfacePackets::SendInterface(StreamBuffer& outStream, ISAACCipher& encryptor, int32_t interfaceId)
{
    LOG_INFO("[PACKET] SendInterface: opcode=238, interfaceId={}", interfaceId);
    outStream.WriteHeader(238, encryptor);
    outStream.WriteInt((548 << 16) | 64, ByteOrder::MIDDLE);
    outStream.WriteShort(interfaceId);
    outStream.WriteByte(0, ValueType::C);
}

void InterfacePackets::SendSidebarInterface(StreamBuffer& outStream, ISAACCipher& encryptor, int32_t menuId, int32_t form)
{
    LOG_INFO("[PACKET] SendSidebarInterface: opcode=238, menuId={}, form={}", menuId, form);
    outStream.WriteHeader(238, encryptor);
    outStream.WriteInt((548 << 16) | menuId, ValueType::STANDARD, ByteOrder::MIDDLE);
    outStream.WriteShort(form);
    outStream.WriteByte(1, ValueType::C);
}

void InterfacePackets::SendWindowPane(StreamBuffer& outStream, ISAACCipher& encryptor, int32_t pane)
{
    LOG_INFO("[PACKET] SendWindowPane: opcode=77, pane={}", pane);
    outStream.WriteHeader(77, encryptor);
    outStream.WriteShort(pane, ValueType::A, ByteOrder::LITTLE);
}

void InterfacePackets::SendComponentPosition(StreamBuffer& outStream, ISAACCipher& encryptor, int32_t interfaceId, int32_t childId, int32_t x, int32_t y)
{
    LOG_INFO("[PACKET] SendComponentPosition: opcode=201, interface={}, child={}, x={}, y={}",
             interfaceId, childId, x, y);
    outStream.WriteHeader(201, encryptor);
    outStream.WriteInt((interfaceId << 16) | childId, ValueType::STANDARD, ByteOrder::LITTLE);
    outStream.WriteShort(y, ValueType::A);
    outStream.WriteShort(x);
}

void InterfacePackets::SendString(StreamBuffer& outStream, ISAACCipher& encryptor, int32_t interfaceId, int32_t childId, const std::string& text)
{
    LOG_INFO("[PACKET] SendString: opcode=47, interface={}, child={}, text=\"{}\"",
             interfaceId, childId, text);
    outStream.WriteHeader(47, encryptor, VariableHeaderSize::SHORT);
    outStream.WriteInt((interfaceId << 16) | childId, ValueType::STANDARD, ByteOrder::MIDDLE);
    outStream.WriteString(text);
    outStream.FinishVariableHeader();
}

void InterfacePackets::SendInterfaceMedia(StreamBuffer& outStream, ISAACCipher& encryptor, int32_t interfaceId, int32_t childId)
{
    LOG_INFO("[PACKET] SendInterfaceMedia: opcode=8, interface={}, child={}", interfaceId, childId);
    outStream.WriteHeader(8, encryptor);
    outStream.WriteInt((interfaceId << 16) | childId, ValueType::STANDARD, ByteOrder::LITTLE);
}

void InterfacePackets::SendAnimateInterface(StreamBuffer& outStream, ISAACCipher& encryptor, int32_t interfaceId, int32_t childId, int32_t animationId)
{
    LOG_INFO("[PACKET] SendAnimateInterface: opcode=63, interface={}, child={}, animation={}", 
             interfaceId, childId, animationId);
    outStream.WriteHeader(63, encryptor);
    outStream.WriteInt((interfaceId << 16) | childId, ValueType::STANDARD, ByteOrder::INVERSE_MIDDLE);
    outStream.WriteShort(animationId, ValueType::STANDARD, ByteOrder::LITTLE);
}

void InterfacePackets::SendInterfaceText(StreamBuffer& outStream, ISAACCipher& encryptor, int32_t interfaceId, int32_t childId, const std::string& text)
{
    LOG_INFO("[PACKET] SendInterfaceText: opcode=114, interface={}, child={}, text=\"{}\"",
             interfaceId, childId, text);
    outStream.WriteHeader(114, encryptor, VariableHeaderSize::SHORT);
    outStream.WriteInt((interfaceId << 16) | childId, ValueType::STANDARD, ByteOrder::INVERSE_MIDDLE);
    outStream.WriteString(text);
    outStream.FinishVariableHeader();
}

void InterfacePackets::SendInterfaceConfig(StreamBuffer& outStream, ISAACCipher& encryptor, int32_t interfaceId, int32_t childId, bool hidden)
{
    LOG_INFO("[PACKET] SendInterfaceConfig: opcode=142, interface={}, child={}, hidden={}", 
             interfaceId, childId, hidden);
    outStream.WriteHeader(142, encryptor);
    outStream.WriteInt((interfaceId << 16) | childId);
    outStream.WriteByte(hidden ? 1 : 0, ValueType::S);
}

void InterfacePackets::CloseInterface(StreamBuffer& outStream, ISAACCipher& encryptor)
{
    LOG_INFO("[PACKET] CloseInterface");
    SendInterface(outStream, encryptor, -1);
}

}