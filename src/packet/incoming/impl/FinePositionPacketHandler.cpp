#include "../../../epch.h"
#include "FinePositionPacketHandler.h"
#include "../../../Player.h"

namespace Skeleton {

void FinePositionPacketHandler::handle(std::shared_ptr<Player> player, StreamBuffer& inStream, int32_t opcode, int32_t length)
{
    // Read fine position (16-bit X, 16-bit Z in high-precision units)
    // High precision = 8x game units (1024 subdivisions per tile)
    int32_t fineX = inStream.ReadShort() & 0xFFFF;
    int32_t fineZ = inStream.ReadShort() & 0xFFFF;

    // Store fine position for broadcasting to nearby players
    player->SetFinePosition(fineX, fineZ);

    LOG_DEBUG("[FINE_POS] Player {} fine position: ({}, {})", player->GetUsername(), fineX, fineZ);
}

}
