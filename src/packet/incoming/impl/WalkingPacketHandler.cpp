#include "../../../epch.h"
#include "WalkingPacketHandler.h"
#include "../../../Player.h"

namespace Skeleton {

void WalkingPacketHandler::handle(std::shared_ptr<Player> player, StreamBuffer& inStream, int32_t opcode, int32_t length) {
    processWalkPacket(player, inStream, opcode, length);
}

void WalkingPacketHandler::processWalkPacket(std::shared_ptr<Player> player, StreamBuffer& inStream, int32_t opcode, int32_t length) {
    LOG_INFO("[WALK PACKET] Received: opcode={}, length={}", opcode, length);

    int32_t steps = 0;
    int32_t firstStepX = 0;
    int32_t firstStepY = 0;
    bool runSteps = false;

    if (opcode == 50 || opcode == 36) {
        steps = (length - 5) / 2;
        std::vector<std::array<int32_t, 2>> path(steps);

        runSteps = inStream.ReadByte(ValueType::S) == 1;
        firstStepY = inStream.ReadShort(ByteOrder::LITTLE);
        firstStepX = inStream.ReadShort(ValueType::A);

        LOG_INFO("[WALK PACKET] First step: x={}, y={}, steps_count={}, run={}",
                 firstStepX, firstStepY, steps, runSteps);

        for (int i = 0; i < steps; ++i) {
            path[i][0] = static_cast<int8_t>(inStream.ReadByte(ValueType::S));
            path[i][1] = static_cast<int8_t>(inStream.ReadByte(ValueType::S));
            LOG_INFO("[WALK PACKET] Step {}: offset_x={}, offset_y={}", i, path[i][0], path[i][1]);
        }

        player->GetMovementHandler().Reset();
        player->GetActionQueue().ClearNonWalkableActions();
        player->GetMovementHandler().SetRunningQueue(runSteps);
        player->GetMovementHandler().AddStep(firstStepX, firstStepY);
        LOG_INFO("[WALK PACKET] Added first step: ({}, {})", firstStepX, firstStepY);

        for (int i = 0; i < steps; ++i) {
            path[i][0] += firstStepX;
            path[i][1] += firstStepY;
            LOG_INFO("[WALK PACKET] Adding waypoint {}: ({}, {})", i, path[i][0], path[i][1]);
            player->GetMovementHandler().AddStep(path[i][0], path[i][1]);
        }

        player->GetMovementHandler().Finish();
    } else if (opcode == 143) {
        int32_t size = length - 14;
        steps = (size - 5) / 2;
        std::vector<std::array<int32_t, 2>> path(steps);

        runSteps = inStream.ReadByte(ValueType::S) == 1;
        firstStepY = inStream.ReadShort(ByteOrder::LITTLE);
        firstStepX = inStream.ReadShort(ValueType::A);

        for (int i = 0; i < steps; ++i) {
            path[i][0] = static_cast<int8_t>(inStream.ReadByte(ValueType::S));
            path[i][1] = static_cast<int8_t>(inStream.ReadByte(ValueType::S));
        }

        player->GetMovementHandler().Reset();
        player->GetActionQueue().ClearNonWalkableActions();
        player->GetMovementHandler().SetRunningQueue(runSteps);
        player->GetMovementHandler().AddStep(firstStepX, firstStepY);

        for (int i = 0; i < steps; ++i) {
            path[i][0] += firstStepX;
            path[i][1] += firstStepY;
            player->GetMovementHandler().AddStep(path[i][0], path[i][1]);
        }

        player->GetMovementHandler().Finish();
    } else if (opcode == 248 || opcode == 164 || opcode == 98) {
        int32_t length2 = length - 14;
        steps = (length2 - 5) / 2;
        std::vector<std::array<int32_t, 2>> path(steps);

        firstStepX = inStream.ReadShort(ValueType::A, ByteOrder::LITTLE);

        for (int i = 0; i < steps; ++i) {
            path[i][0] = static_cast<int8_t>(inStream.ReadByte());
            path[i][1] = static_cast<int8_t>(inStream.ReadByte());
        }

        firstStepY = inStream.ReadShort(ByteOrder::LITTLE);
        int32_t runFlag = inStream.ReadByte(ValueType::C);

        player->GetMovementHandler().Reset();
        player->GetActionQueue().ClearNonWalkableActions();
        player->GetMovementHandler().SetRunningQueue(runFlag == 1);
        player->GetMovementHandler().AddStep(firstStepX, firstStepY);

        for (int i = 0; i < steps; ++i) {
            path[i][0] += firstStepX;
            path[i][1] += firstStepY;
            player->GetMovementHandler().AddStep(path[i][0], path[i][1]);
        }

        player->GetMovementHandler().Finish();
    }

    LOG_INFO("[WALK PACKET] Processed successfully");
}

}
