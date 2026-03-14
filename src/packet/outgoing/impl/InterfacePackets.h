#pragma once
#include "../../../StreamBuffer.h"
#include "../../../ISAACCipher.h"

namespace Skeleton {

class InterfacePackets {
public:
    static void SendInterface(StreamBuffer& outStream, ISAACCipher& encryptor, int32_t interfaceId);  // Packet 238 - Display interface to user
    static void SendSidebarInterface(StreamBuffer& outStream, ISAACCipher& encryptor, int32_t menuId, int32_t form);  // Packet 238 - Display sidebar tab interface
    static void SendWindowPane(StreamBuffer& outStream, ISAACCipher& encryptor, int32_t pane);  // Packet 77 - Set window pane
    static void SendComponentPosition(StreamBuffer& outStream, ISAACCipher& encryptor, int32_t interfaceId, int32_t childId, int32_t x, int32_t y);  // Packet 201 - Set interface component position
    static void SendString(StreamBuffer& outStream, ISAACCipher& encryptor, int32_t interfaceId, int32_t childId, const std::string& text);  // Packet 47 - Set interface string text
    static void SendInterfaceMedia(StreamBuffer& outStream, ISAACCipher& encryptor, int32_t interfaceId, int32_t childId);  // Packet 8 - Display player head in dialogue
    static void SendAnimateInterface(StreamBuffer& outStream, ISAACCipher& encryptor, int32_t interfaceId, int32_t childId, int32_t animationId);  // Packet 63 - Animate interface component
    static void SendInterfaceText(StreamBuffer& outStream, ISAACCipher& encryptor, int32_t interfaceId, int32_t childId, const std::string& text);  // Packet 114 - Set interface text (alternate)
    static void SendInterfaceConfig(StreamBuffer& outStream, ISAACCipher& encryptor, int32_t interfaceId, int32_t childId, bool hidden);  // Packet 142 - Show/hide interface component
    static void CloseInterface(StreamBuffer& outStream, ISAACCipher& encryptor);  // Packet 238 - Close open interface
};

}
