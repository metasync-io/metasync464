#include "../../../epch.h"
#include "CommandPacketHandler.h"
#include "../../../Player.h"

namespace Skeleton {

void CommandPacketHandler::handle(std::shared_ptr<Player> player, StreamBuffer& inStream, int32_t opcode, int32_t length) {
    LOG_INFO("Command packet (165) received: length={}", length);

    if (length > 0) {
        std::string commandLine = inStream.ReadString();
        LOG_INFO("Processing command: '{}'", commandLine);

        if (player) {
            player->ProcessCommand(commandLine);
        } else {
            LOG_ERROR("Cannot process command: no player associated with client");
        }
    }
}

}
