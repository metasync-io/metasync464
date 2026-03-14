#include "ChatPacketHandler.h"
#include "../../../Player.h"
#include "../../../TextUtils.h"
namespace Skeleton {

void ChatPacketHandler::handle(std::shared_ptr<Player> player, StreamBuffer& inStream, int32_t opcode, int32_t length) {
    
    int32_t effects = inStream.ReadByte() & 0xFF;
    int32_t colour = inStream.ReadByte() & 0xFF;
    int32_t size = inStream.ReadByte() & 0xFF;

    if (player->GetChatMessageQueue().size() >= 4) {
        return;
    }

    std::string unpacked = TextUtils::DecryptPlayerChat(inStream, size);
    unpacked = TextUtils::FilterText(unpacked);
    unpacked = TextUtils::OptimizeText(unpacked);

    LOG_INFO("[ChatPacketHandler::handle] Player '{}' says: '{}'", player->GetUsername(), unpacked);

    player->GetChatMessageQueue().push(ChatMessage(colour, size, unpacked, effects));
}

}
