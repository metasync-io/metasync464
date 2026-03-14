#pragma once
#include <bitset>

#include "Constants.h"
#include "Player.h"
#include "PlayerUpdating.h"
#include "NPCUpdating.h"
#include "EntityList.h"
#include "EventManager.h"

namespace Skeleton {

    class PlayerUpdating;
    class GameEngine;

    class World {
    public:

        static World& Instance();

        void init(GameEngine& engine);
        
        // Explicit shutdown to ensure proper cleanup before destruction
        void shutdown();

        GameEngine& getEngine() { return *m_Engine; }

        void Process();

        // High-frequency face direction updates (called every 50ms)
        void ProcessFastUpdates();

        void RegisterPlayer(std::shared_ptr<Player> player);

        void RemovePlayer(const std::string& username);

        std::shared_ptr<Player> GetPlayer(const std::string& username);

        EntityList<Player, MAX_PLAYERS>& GetPlayers() { return m_Players; }
        const EntityList<Player, MAX_PLAYERS>& GetPlayers() const { return m_Players; }

        bool isPlayerOnline(const std::string& username);

        void submit(std::shared_ptr<Event> event);

        EventManager& getEventManager() { return *m_EventManager; }
        
        bool isInitialized() const { return m_Initialized; }

    private:

        World() = default;

        void registerPacketHandlers();
        void registerGlobalEvents();

        std::atomic<bool> m_Initialized{false};
        std::atomic<bool> m_ShuttingDown{false};
        GameEngine* m_Engine = nullptr;

        std::unique_ptr<EventManager> m_EventManager;

        EntityList<Player, MAX_PLAYERS> m_Players;
        mutable std::mutex m_PlayersMutex;

        std::unordered_map<std::string, std::shared_ptr<Player>> m_PlayersByName;
        mutable std::mutex m_PlayersByNameMutex;  // Separate mutex for name map

        PlayerUpdating m_PlayerUpdating;

        NPCUpdating m_NPCUpdating;
    };

}
