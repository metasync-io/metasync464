#pragma once
#include "epch.h"
#include "StreamBuffer.h"

namespace Skeleton {
    class Player;
    class NPC;

    class NPCUpdating {
    public:
        NPCUpdating() = default;
        ~NPCUpdating() = default;

        void Update(std::vector<std::shared_ptr<Player>> players, bool logSummary = false);

    private:
        void UpdateNPCsForPlayer(std::shared_ptr<Player> player);
        void AddNPC(StreamBuffer& out, std::shared_ptr<Player> player, std::shared_ptr<NPC> npc);
        void UpdateNPCMovement(std::shared_ptr<NPC> npc, StreamBuffer& out);
        void UpdateNPCState(std::shared_ptr<NPC> npc, StreamBuffer& block);
    };
}
