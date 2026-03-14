#include "epch.h"
#include "NPCUpdating.h"
#include "Player.h"
#include "World.h"
#include "Client.h"

namespace Skeleton {

    void NPCUpdating::Update(std::vector<std::shared_ptr<Player>> players, bool logSummary) {
        for (const auto& player : players) {
            if (player && player->GetClient()) {
                UpdateNPCsForPlayer(player);
            }
        }
    }

    void NPCUpdating::UpdateNPCsForPlayer(std::shared_ptr<Player> player) {

        StreamBuffer out(2048);
        StreamBuffer block(1024);

        out.WriteHeader(69, *player->GetClient()->GetEncryptor(), VariableHeaderSize::SHORT);
        out.SetAccessType(AccessType::BIT_ACCESS);

        out.WriteBits(8, 0);

        out.WriteBits(14, 0x3FFF);
        out.SetAccessType(AccessType::BYTE_ACCESS);

        out.FinishVariableHeader();
        player->GetClient()->Send(out);
    }

    void NPCUpdating::AddNPC(StreamBuffer& out, std::shared_ptr<Player> player, std::shared_ptr<NPC> npc) {

    }

    void NPCUpdating::UpdateNPCMovement(std::shared_ptr<NPC> npc, StreamBuffer& out) {

    }

    void NPCUpdating::UpdateNPCState(std::shared_ptr<NPC> npc, StreamBuffer& block) {

    }
}
