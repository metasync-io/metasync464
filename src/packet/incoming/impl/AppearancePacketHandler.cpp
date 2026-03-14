#include "../../../epch.h"
#include "AppearancePacketHandler.h"
#include "../../../Player.h"

namespace Skeleton {

void AppearancePacketHandler::handle(std::shared_ptr<Player> player, StreamBuffer& inStream, int32_t opcode, int32_t length) {
    LOG_INFO("[APPEARANCE] Received appearance change packet, length={}", length);

    if (length == 13) {
        uint8_t gender = inStream.ReadByte();
        uint8_t head = inStream.ReadByte();
        uint8_t beard = inStream.ReadByte();
        uint8_t chest = inStream.ReadByte();
        uint8_t arms = inStream.ReadByte();
        uint8_t hands = inStream.ReadByte();
        uint8_t legs = inStream.ReadByte();
        uint8_t feet = inStream.ReadByte();
        uint8_t hairColour = inStream.ReadByte();
        uint8_t torsoColour = inStream.ReadByte();
        uint8_t legColour = inStream.ReadByte();
        uint8_t feetColour = inStream.ReadByte();
        uint8_t skinColour = inStream.ReadByte();

        LOG_INFO("[APPEARANCE] gender={}, head={}, beard={}, chest={}, arms={}, hands={}, legs={}, feet={}",
            gender, head, beard, chest, arms, hands, legs, feet);
        LOG_INFO("[APPEARANCE] colors: hair={}, torso={}, leg={}, feet={}, skin={}",
            hairColour, torsoColour, legColour, feetColour, skinColour);

        player->SetAppearance(gender, head, beard, chest, arms, hands, legs, feet,
                              hairColour, torsoColour, legColour, feetColour, skinColour);
        player->SetUpdateFlag(UpdateFlag::All);
    } else {
        LOG_ERROR("[APPEARANCE] Invalid packet length: {}, expected 13", length);
    }
}

}
