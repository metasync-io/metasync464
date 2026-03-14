#include "../../../epch.h"
#include "FaceDirectionPacketHandler.h"
#include "../../../Player.h"

namespace Skeleton {

void FaceDirectionPacketHandler::handle(std::shared_ptr<Player> player, StreamBuffer& inStream, int32_t opcode, int32_t length)
{
    // Read face direction (0-2047 range, same as client's turnDirection)
    int32_t direction = inStream.ReadShort();

    // Queue the face direction update for processing during the game tick
    // This ensures the flag isn't cleared by a race condition between
    // the packet processing thread and the pulse thread's Reset() call
    player->QueueFaceDirection(direction);

    LOG_DEBUG("[FACE] Player {} queued face direction {}", player->GetUsername(), direction);
}

}
