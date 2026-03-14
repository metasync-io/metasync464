#include "EnterAmountPacketHandler.h"

namespace Skeleton {

void EnterAmountPacketHandler::handle(std::shared_ptr<Player> player, StreamBuffer& inStream, int32_t opcode, int32_t length) {
    int32_t amount = inStream.ReadInt();
}

}
