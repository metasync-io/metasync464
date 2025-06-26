#include "epch.h"
#include "World.h"

namespace Skeleton {

    World& World::Instance() 
    {
        static World instance;
        return instance;
    }

    void World::Process()
    {
        std::vector<std::shared_ptr<Player>> playerList;
        for (auto& [name, player] : GetPlayers()) 
        {
            player->Process();
            playerList.push_back(player);
        }

        m_PlayerUpdating.Update(playerList);

        for (auto& player : playerList) 
        {
            player->Reset();
        }
    }

    void World::RegisterPlayer(std::shared_ptr<Player> player)
    {

        int32_t index = GetFreeIndex();
        if (index == -1) {
            LOG_ERROR("Failed to register player {}: world is full!", player->GetUsername());
            return;
        }
        player->SetIndex(index);

        m_Players[player->GetUsername()] = player;
        player->Login();

        LOG_INFO("Registered new player: {} Index: {}", player->GetUsername(), player->GetIndex());
    }

    void World::RemovePlayer(const std::string& username) 
    {
        auto player = GetPlayer(username);
        if (player) {
            int32_t slot = player->GetIndex();
            if (slot != -1) {
                m_OccupiedIndices.reset(slot);
            }

            player->Logout();
            m_Players.erase(username);
            LOG_INFO("Removed player: {}", username);
        }
        else {
            LOG_WARN("Tried to remove non-existing player!");
        }
    }

    std::shared_ptr<Player> World::GetPlayer(const std::string& username) 
    {
        auto it = m_Players.find(username);
        return (it != m_Players.end()) ? it->second : nullptr;
    }

    int32_t World::GetFreeIndex()
    {
        for (int i = 0; i < MAX_PLAYERS; ++i) {
            if (!m_OccupiedIndices.test(i)) {
                m_OccupiedIndices.set(i);
                return i;
            }
        }
        return -1; // No free slots
    }

}