#include "../../../epch.h"
#include "MoveForwardPacketHandler.h"
#include "../../../Player.h"
#include <cmath>

namespace Skeleton {

void MoveForwardPacketHandler::handle(std::shared_ptr<Player> player, StreamBuffer& inStream, int32_t opcode, int32_t length)
{
    // MoveStart packet (opcode 201, 0 bytes) - start continuous movement
    if (!player->IsAnyCameraMoveMode()) {
        LOG_DEBUG("[MOVE_START] Player {} not in camera mode, ignoring", player->GetUsername());
        return;
    }

    // Calculate direction from face coordinates
    int32_t playerX = player->GetPosition().GetX();
    int32_t playerY = player->GetPosition().GetY();
    int32_t faceX = player->GetFaceX();
    int32_t faceY = player->GetFaceY();

    int32_t centerX = playerX * 2 + 1;
    int32_t centerY = playerY * 2 + 1;
    int32_t dx = faceX - centerX;
    int32_t dy = faceY - centerY;

    double angle = std::atan2(static_cast<double>(dx), static_cast<double>(dy));
    double degrees = angle * 180.0 / 3.14159265358979;
    if (degrees < 0) degrees += 360.0;

    int32_t sector = static_cast<int32_t>((degrees + 22.5) / 45.0) % 8;
    static const int32_t SECTOR_TO_MOVEMENT[] = {1, 2, 4, 7, 6, 5, 3, 0};
    int32_t movementDir = SECTOR_TO_MOVEMENT[sector];

    // Enable continuous movement
    player->SetCameraLockMoving(true);
    player->SetCameraLockMoveDirection(movementDir);

    LOG_INFO("[MOVE_START] Player {} dir={}", player->GetUsername(), movementDir);
}

}
