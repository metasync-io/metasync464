#include "../../../epch.h"
#include "Turn180PacketHandler.h"
#include "../../../Player.h"
#include <cmath>
#include <vector>

namespace Skeleton {

void Turn180PacketHandler::handle(std::shared_ptr<Player> player, StreamBuffer& inStream, int32_t opcode, int32_t length)
{
    // Only process if any camera movement mode is enabled
    if (!player->IsAnyCameraMoveMode()) {
        LOG_DEBUG("[TURN180] Player {} not in camera mode, ignoring", player->GetUsername());
        return;
    }

    // Check if player is moving - Turn180 only works when idle
    auto& movementHandler = player->GetMovementHandler();
    if (movementHandler.GetQueueSize() > 1) {
        LOG_DEBUG("[TURN180] Player {} is moving, ignoring turn180", player->GetUsername());
        return;
    }

    // Calculate current face direction from face coordinates
    int32_t playerX = player->GetPosition().GetX();
    int32_t playerY = player->GetPosition().GetY();
    int32_t faceX = player->GetFaceX();
    int32_t faceY = player->GetFaceY();

    // Calculate direction vector
    int32_t centerX = playerX * 2 + 1;
    int32_t centerY = playerY * 2 + 1;
    int32_t dx = faceX - centerX;
    int32_t dy = faceY - centerY;

    // Convert to angle (radians) using atan2
    double angle = std::atan2(static_cast<double>(-dx), static_cast<double>(-dy));

    // Convert to 0-2047 direction range
    // Angle 0 = south, so we need to adjust
    constexpr double PI = 3.14159265358979;
    double normalizedAngle = angle;
    if (normalizedAngle < 0) normalizedAngle += 2.0 * PI;

    int32_t currentDir = static_cast<int32_t>(normalizedAngle * 2048.0 / (2.0 * PI)) % 2048;

    // Calculate target direction (opposite = +1024)
    int32_t targetDir = (currentDir + 1024) % 2048;

    // Generate intermediate directions for smooth rotation
    // Using 4 intermediate steps over ~200ms (5 total directions at 50ms each = 250ms)
    constexpr int32_t NUM_STEPS = 5;
    std::vector<int32_t> directions;
    directions.reserve(NUM_STEPS);

    for (int i = 1; i <= NUM_STEPS; i++) {
        // Linear interpolation from current to target
        int32_t stepDir = currentDir + (1024 * i / NUM_STEPS);
        stepDir = stepDir % 2048;
        directions.push_back(stepDir);
    }

    // Queue all directions for processing
    player->QueueMultipleFaceDirections(directions);

    LOG_INFO("[TURN180] Player {} turning 180: {} -> {} ({} steps)",
             player->GetUsername(), currentDir, targetDir, NUM_STEPS);
}

}
