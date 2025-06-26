#pragma once
#include <bitset>

#include "Constants.h"
#include "Player.h"
#include "PlayerUpdating.h"

namespace Skeleton {

    class PlayerUpdating;

    class World {
    public:
        static World& Instance();

        void Process();

        void RegisterPlayer(std::shared_ptr<Player> player);
        void RemovePlayer(const std::string& username);
        std::shared_ptr<Player> GetPlayer(const std::string& username);
        const std::unordered_map<std::string, std::shared_ptr<Player>>& GetPlayers() const { return m_Players;}
    private:
        World() = default;

        int32_t GetFreeIndex();

        std::unordered_map<std::string, std::shared_ptr<Player>> m_Players;
        PlayerUpdating m_PlayerUpdating;
        std::bitset<MAX_PLAYERS> m_OccupiedIndices; // tracks which slots are in use
    };

}