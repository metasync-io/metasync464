#include "../../../epch.h"
#include "BrakeMovementPacketHandler.h"
#include "../../../Player.h"

namespace Skeleton {

void BrakeMovementPacketHandler::handle(std::shared_ptr<Player> player, StreamBuffer& inStream, int32_t opcode, int32_t length)
{
    // MoveStop packet (opcode 205, 0 bytes) - stop continuous movement
    if (!player->IsAnyCameraMoveMode()) {
        LOG_DEBUG("[MOVE_STOP] Player {} not in camera mode, ignoring", player->GetUsername());
        return;
    }

    // Stop continuous movement
    player->SetCameraLockMoving(false);
    player->SetCameraLockMoveDirection(-1);

    LOG_INFO("[MOVE_STOP] Player {} stopped", player->GetUsername());
}

}
