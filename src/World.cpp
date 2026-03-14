#include "epch.h"
#include "World.h"
#include "NPCUpdating.h"
#include "GameEngine.h"
#include "UpdateEvent.h"
#include "packet/incoming/PacketManager.h"
#include "packet/incoming/impl/WalkingPacketHandler.h"
#include "packet/incoming/impl/CommandPacketHandler.h"
#include "packet/incoming/impl/ActionButtonPacketHandler.h"
#include "packet/incoming/impl/ChatPacketHandler.h"
#include "packet/incoming/impl/CloseInterfacePacketHandler.h"
#include "packet/incoming/impl/EnterAmountPacketHandler.h"
#include "packet/incoming/impl/InterfaceOptionPacketHandler.h"
#include "packet/incoming/impl/ItemOptionPacketHandler.h"
#include "packet/incoming/impl/NPCInteractPacketHandler.h"
#include "packet/incoming/impl/ObjectOptionPacketHandler.h"
#include "packet/incoming/impl/PlayerOptionPacketHandler.h"
#include "packet/incoming/impl/SwitchItemPacketHandler.h"
#include "packet/incoming/impl/WieldPacketHandler.h"
#include "packet/incoming/impl/AppearancePacketHandler.h"
#include "packet/incoming/impl/FaceDirectionPacketHandler.h"
#include "packet/incoming/impl/FinePositionPacketHandler.h"
#include "packet/incoming/impl/MoveForwardPacketHandler.h"
#include "packet/incoming/impl/BrakeMovementPacketHandler.h"
#include "packet/incoming/impl/Turn180PacketHandler.h"
// Note: PlayerTickTask, PlayerUpdateTask, PlayerResetTask removed - 
// World::Process() now runs synchronously instead of queueing tasks

namespace Skeleton {

World& World::Instance()
{
    static World instance;
    return instance;
}

void World::init(GameEngine& engine)
{
    LOG_INFO("World::init() - Starting initialization");
    if (m_Engine != nullptr) {
        throw std::runtime_error("World already initialized");
    }
    
    LOG_INFO("World::init() - Setting engine pointer");
    m_Engine = &engine;
    LOG_INFO("World::init() - Creating EventManager");
    m_EventManager = std::make_unique<EventManager>(engine);
    
    LOG_INFO("World::init() - Registering packet handlers");
    registerPacketHandlers();
    LOG_INFO("World::init() - Registering global events");
    registerGlobalEvents();
    
    LOG_INFO("World::init() - Setting initialized flag to true");
    m_Initialized = true;
    LOG_INFO("World::init() - Initialization complete");
}

void World::shutdown()
{
    LOG_INFO("World::shutdown() - Starting shutdown");
    
    // Set shutting down flag first to prevent new operations
    m_ShuttingDown = true;
    m_Initialized = false;
    
    // Clear all players under lock
    {
        std::lock_guard<std::mutex> lock(m_PlayersMutex);
        std::lock_guard<std::mutex> nameLock(m_PlayersByNameMutex);
        
        // First log out all players
        // Note: Use 'auto' (not 'auto&') because EntityList iterator returns shared_ptr by value
        for (auto player : m_Players) {
            if (player) {
                try {
                    player->Logout();
                } catch (const std::exception& e) {
                    LOG_ERROR("Error logging out player: {}", e.what());
                }
            }
        }
        
        // Clear the name map
        m_PlayersByName.clear();
    }
    
    // Clear event manager
    if (m_EventManager) {
        m_EventManager.reset();
    }
    
    m_Engine = nullptr;
    
    LOG_INFO("World::shutdown() - Shutdown complete");
}

void World::registerPacketHandlers()
{
    auto& pm = PacketManager::getInstance();
    
    // CLIENT -> SERVER PACKETS
    
    // Walking packets - Player movement
    auto walkingHandler = std::make_shared<WalkingPacketHandler>();
    pm.bind(50, walkingHandler);   // Walk packet variant 1
    pm.bind(36, walkingHandler);   // Walk packet variant 2
    pm.bind(143, walkingHandler);  // Walk packet variant 3
    pm.bind(248, walkingHandler);  // Walk packet variant 4
    pm.bind(164, walkingHandler);  // Walk packet variant 5
    pm.bind(98, walkingHandler);   // Walk packet variant 6
    
    // Command packet - Player commands (::tele, ::logout, etc.)
    auto commandHandler = std::make_shared<CommandPacketHandler>();
    pm.bind(165, commandHandler);  // Command packet
    
    // Action button packets - Interface button clicks
    auto actionButtonHandler = std::make_shared<ActionButtonPacketHandler>();
    pm.bind(113, actionButtonHandler);  // Click button variant 1
    pm.bind(153, actionButtonHandler);  // Click button variant 2
    pm.bind(240, actionButtonHandler);  // Click button variant 3
    
    // Chat packet - Player chat messages
    auto chatHandler = std::make_shared<ChatPacketHandler>();
    pm.bind(115, chatHandler);  // Player chat
    
    // Interface packets
    auto closeInterfaceHandler = std::make_shared<CloseInterfacePacketHandler>();
    pm.bind(255, closeInterfaceHandler);  // Close interface
    
    auto enterAmountHandler = std::make_shared<EnterAmountPacketHandler>();
    pm.bind(208, enterAmountHandler);  // Enter amount dialog
    
    auto interfaceOptionHandler = std::make_shared<InterfaceOptionPacketHandler>();
    pm.bind(177, interfaceOptionHandler);  // Interface option 1
    pm.bind(88, interfaceOptionHandler);   // Interface option 2
    pm.bind(159, interfaceOptionHandler);  // Interface option 3
    pm.bind(86, interfaceOptionHandler);   // Interface option 4
    
    // Item interaction packets
    auto itemOptionHandler = std::make_shared<ItemOptionPacketHandler>();
    pm.bind(145, itemOptionHandler);  // Item option 1
    pm.bind(117, itemOptionHandler);  // Item option 2
    pm.bind(43, itemOptionHandler);   // Item option 3
    pm.bind(129, itemOptionHandler);  // Item option 4
    pm.bind(135, itemOptionHandler);  // Item option 5
    pm.bind(101, itemOptionHandler);  // Item click
    pm.bind(247, itemOptionHandler);  // Item drop
    pm.bind(216, itemOptionHandler);  // Pickup ground item
    
    // NPC interaction packets
    auto npcInteractHandler = std::make_shared<NPCInteractPacketHandler>();
    pm.bind(156, npcInteractHandler);  // NPC talk-to
    pm.bind(19, npcInteractHandler);   // NPC trade
    pm.bind(72, npcInteractHandler);   // NPC examine
    
    // Object interaction packets
    auto objectOptionHandler = std::make_shared<ObjectOptionPacketHandler>();
    pm.bind(44, objectOptionHandler);   // Object click 1
    pm.bind(252, objectOptionHandler);  // Object click 2
    
    // Player interaction packets
    auto playerOptionHandler = std::make_shared<PlayerOptionPacketHandler>();
    pm.bind(128, playerOptionHandler);  // Player option 1
    pm.bind(37, playerOptionHandler);   // Player option 2
    pm.bind(227, playerOptionHandler);  // Player option 3
    
    // Equipment packets
    auto switchItemHandler = std::make_shared<SwitchItemPacketHandler>();
    pm.bind(214, switchItemHandler);  // Switch item slots
    
    auto wieldHandler = std::make_shared<WieldPacketHandler>();
    pm.bind(215, wieldHandler);  // Equip/wield item
    
    // Character customization
    auto appearanceHandler = std::make_shared<AppearancePacketHandler>();
    pm.bind(157, appearanceHandler);  // Character appearance change

    // Face direction packet - Camera-lock facing updates
    auto faceDirectionHandler = std::make_shared<FaceDirectionPacketHandler>();
    pm.bind(200, faceDirectionHandler);  // Face direction update

    // Fine position packet - Sub-tile smooth movement sync
    auto finePositionHandler = std::make_shared<FinePositionPacketHandler>();
    pm.bind(206, finePositionHandler);  // Fine position update (4 bytes: X, Z)

    // Camera-lock movement control packets
    auto moveStartHandler = std::make_shared<MoveForwardPacketHandler>();
    pm.bind(201, moveStartHandler);  // MoveStart (0 bytes)

    auto moveStopHandler = std::make_shared<BrakeMovementPacketHandler>();
    pm.bind(205, moveStopHandler);   // MoveStop (0 bytes)

    auto turn180Handler = std::make_shared<Turn180PacketHandler>();
    pm.bind(203, turn180Handler);  // 180-degree turn

    LOG_INFO("Registered packet handlers");
}

void World::registerGlobalEvents()
{
    submit(std::make_shared<UpdateEvent>());
}

void World::submit(std::shared_ptr<Event> event)
{
    if (m_ShuttingDown || !m_EventManager) {
        return;
    }
    m_EventManager->submit(event);
}

void World::Process()
{
    // Check shutdown and initialized flags atomically
    if (m_ShuttingDown || !m_Initialized) {
        return;
    }

    std::vector<std::shared_ptr<Player>> playerList;

    {
        std::lock_guard<std::mutex> lock(m_PlayersMutex);
        // Double check after acquiring lock
        if (m_ShuttingDown) {
            return;
        }
        
        for (auto player : GetPlayers())
        {
            if (player) {
                playerList.push_back(player);
            }
        }
    }

    // CRITICAL FIX: Execute synchronously like the original working version
    // The task-based async approach broke the game loop because:
    // 1. Tasks were queued to run on Logic Thread
    // 2. But World::Process() runs on Pulse Thread  
    // 3. Operations executed out of order (PlayerUpdateTask after Reset!)
    // 4. Client got malformed/missing player update packets
    //
    // The original working code was synchronous:
    // 1. Process each player (movement, chat queue)
    // 2. Send player updates to all clients
    // 3. Send NPC updates to all clients
    // 4. Reset player state for next tick
    
    // Step 1: Process each player (movement, queued actions, etc.)
    for (auto& player : playerList) {
        if (player) {
            player->Process();
        }
    }

    // Step 2: Send player update packets to all clients
    uint64_t currentTick = m_Engine->getTickCount();
    bool shouldLogSummary = (currentTick % 250 == 0);
    
    m_PlayerUpdating.Update(playerList, shouldLogSummary);

    // Step 3: Send NPC update packets to all clients
    m_NPCUpdating.Update(playerList, shouldLogSummary);
    
    // Log connection summary every 250 ticks
    if (shouldLogSummary && !playerList.empty()) {
        std::unordered_set<std::string> uniqueIPs;
        for (const auto& player : playerList) {
            try {
                std::string ip = player->GetClient()->m_Socket.remote_endpoint().address().to_string();
                uniqueIPs.insert(ip);
            } catch (const std::exception&) {}
        }
        LOG_INFO("\033[96m[SERVER]\033[0m Total client connections: {} | Total IP connections: {}", 
            playerList.size(), uniqueIPs.size());
    }

    // Step 4: Reset player state for next tick
    for (auto& player : playerList) {
        if (player) {
            player->Reset();
        }
    }

    // Process scheduled events (if any)
    if (m_EventManager && !m_ShuttingDown) {
        m_EventManager->process();
    }
}

void World::ProcessFastUpdates()
{
    // High-frequency updates for face direction AND movement (runs every 50ms)
    // This bypasses the normal 600ms tick for smoother sync

    if (m_ShuttingDown || !m_Initialized) {
        return;
    }

    std::vector<std::shared_ptr<Player>> playerList;
    {
        std::lock_guard<std::mutex> lock(m_PlayersMutex);
        if (m_ShuttingDown) {
            return;
        }

        for (auto player : GetPlayers()) {
            if (player) {
                playerList.push_back(player);
            }
        }
    }

    // Process movement and face direction updates for each player
    for (auto& player : playerList) {
        if (player) {
            // Process fast packets first (MoveForward/BrakeMovement via fast channel)
            player->ProcessFastPackets();

            // IMPORTANT: Process face direction FIRST (from camera lock packets)
            // This ensures movement calculation uses the latest face coordinates
            // when the player is rotating while moving in chase camera mode
            bool hadFaceUpdate = player->ProcessFastFaceUpdate();

            // Process movement (timer-based, respects WALK_DELAY_MS)
            // Uses m_FaceX/m_FaceY which was just updated above
            int32_t primaryDir, secondaryDir;
            bool movementCompleted = false;
            bool hadMovement = player->ProcessFastMovement(primaryDir, secondaryDir, movementCompleted);

            // Send updates to observers
            if (hadMovement) {
                m_PlayerUpdating.SendMovementUpdate(player, playerList, primaryDir, secondaryDir);

                // If movement just completed, also send face direction update
                // so other players see the final facing direction
                if (movementCompleted) {
                    m_PlayerUpdating.SendFaceUpdate(player, playerList);
                }
            }
            if (hadFaceUpdate) {
                m_PlayerUpdating.SendFaceUpdate(player, playerList);
            }

            // Send fine position updates for sub-tile smoothing
            if (player->HasFinePosition()) {
                m_PlayerUpdating.SendFinePositionUpdate(player, playerList);
                player->ClearFinePosition();  // Clear after sending to prevent re-sending same position
            }
        }
    }
}

void World::RegisterPlayer(std::shared_ptr<Player> player)
{
    if (m_ShuttingDown) {
        LOG_WARN("Cannot register player during shutdown");
        return;
    }
    
    {
        std::lock_guard<std::mutex> nameLock(m_PlayersByNameMutex);
        if (m_PlayersByName.find(player->GetUsername()) != m_PlayersByName.end()) {
            LOG_WARN("Player {} is already online", player->GetUsername());
            return;
        }
    }

    {
        std::lock_guard<std::mutex> lock(m_PlayersMutex);
        if (!m_Players.add(player)) {
            LOG_ERROR("Could not register player {} - world full", player->GetUsername());
            return;
        }
    }

    {
        std::lock_guard<std::mutex> nameLock(m_PlayersByNameMutex);
        m_PlayersByName[player->GetUsername()] = player;
    }

    player->Login();

    LOG_INFO("Registered new player: {} Index: {}", player->GetUsername(), player->GetIndex());
}

void World::RemovePlayer(const std::string& username)
{
    std::shared_ptr<Player> player;
    
    {
        std::lock_guard<std::mutex> nameLock(m_PlayersByNameMutex);
        auto it = m_PlayersByName.find(username);
        if (it != m_PlayersByName.end()) {
            player = it->second;
        }
    }

    if (player) {
        player->Logout();

        {
            std::lock_guard<std::mutex> lock(m_PlayersMutex);
            m_Players.remove(player);
        }
        
        {
            std::lock_guard<std::mutex> nameLock(m_PlayersByNameMutex);
            m_PlayersByName.erase(username);
        }

        LOG_INFO("Removed player: {}", username);
    }
    else {
        LOG_WARN("Tried to remove non-existing player: {}", username);
    }
}

std::shared_ptr<Player> World::GetPlayer(const std::string& username)
{
    std::lock_guard<std::mutex> nameLock(m_PlayersByNameMutex);
    auto it = m_PlayersByName.find(username);
    return (it != m_PlayersByName.end()) ? it->second : nullptr;
}

bool World::isPlayerOnline(const std::string& username)
{
    std::lock_guard<std::mutex> nameLock(m_PlayersByNameMutex);
    return m_PlayersByName.find(username) != m_PlayersByName.end();
}

}
