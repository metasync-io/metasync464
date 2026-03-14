#include "epch.h"
#include "Player.h"
#include "World.h"
#include "packet/outgoing/impl/InterfacePackets.h"
#include "packet/outgoing/impl/PlayerPackets.h"
#include "packet/outgoing/impl/WorldPackets.h"
#include "packet/outgoing/impl/SystemPackets.h"
#include <sstream>
#include <algorithm>
#include <cmath>
#include <random>

namespace Skeleton {

    Player::Player(const std::string& username, std::weak_ptr<Client> client)
        : m_Username(username), m_Client(client), m_Position(3099, 3107, 0), m_MovementHandler(std::make_unique<MovementHandler>(*this)), m_EquipmentHandler(std::make_unique<EquipmentHandler>(*this)), m_CombatHandler(std::make_unique<CombatHandler>(*this)), m_SkillHandler(std::make_unique<SkillHandler>(*this)), m_Inventory(std::make_unique<Inventory>(*this)), m_HitQueue(*this)
    {
        m_Equipment.fill(0);

        // Initialize hitpoints to full HP (based on hitpoints level)
        m_Hitpoints = m_SkillHandler->GetEffectiveLevel(Skill::HITPOINTS);
    }

    Player::~Player() = default;

    const std::string& Player::GetUsername() const
    {
        return m_Username;
    }

    void Player::Logout()
    {
        LOG_INFO("Player logout");
    }

    void Player::Process()
    {
        // Movement is now handled by ProcessFastMovement() on the fast pulse thread
        // Process() only handles teleporting for movement
        m_MovementHandler->Process();

        m_CombatHandler->Process();
        m_HitQueue.Process();

        // Note: Face direction updates are handled by ProcessFastFaceUpdate()
        // which runs on the fast pulse thread (50ms) for real-time synchronization

        if (!m_ChatMessageQueue.empty() && !m_CurrentChatMessage) {
            m_CurrentChatMessage = std::make_unique<ChatMessage>(m_ChatMessageQueue.front());
            m_ChatMessageQueue.pop();
            SetUpdateFlag(UpdateFlag::Chat);
            LOG_INFO("[CHAT] Set current chat message for '{}': '{}'", m_Username, m_CurrentChatMessage->chatText);
        }
    }

    bool Player::ProcessFastFaceUpdate()
    {
        // Process pending face direction update (called from fast pulse thread at 50ms intervals)
        int32_t pendingDir = -1;
        {
            std::lock_guard<std::mutex> lock(m_FaceDirectionMutex);
            if (!m_PendingFaceDirections.empty()) {
                pendingDir = m_PendingFaceDirections.front();
                m_PendingFaceDirections.pop();
            }
        }

        if (pendingDir < 0) {
            return false;  // No pending update
        }

        // Convert direction angle to face coordinates
        // Direction 0 = south, 512 = west, 1024 = north, 1536 = east
        int32_t playerX = GetPosition().GetX();
        int32_t playerY = GetPosition().GetY();

        // Convert to radians (direction uses 0-2047 range)
        constexpr double PI = 3.14159265358979;
        double angle = pendingDir * 2.0 * PI / 2048.0;

        // Calculate face coordinate offset with high precision
        // Using a large multiplier (512) to preserve angular resolution across all 2048 direction bins
        // The client will calculate direction from the delta between player pos and face coord
        // For RS coordinate system: -sin for X (west is negative X), -cos for Y (south is negative Y)
        constexpr int32_t FACE_DISTANCE = 512;
        int32_t faceX = playerX * 2 + 1 - static_cast<int32_t>(std::sin(angle) * FACE_DISTANCE);
        int32_t faceY = playerY * 2 + 1 - static_cast<int32_t>(std::cos(angle) * FACE_DISTANCE);

        SetFaceCoordinate(faceX, faceY);

        LOG_DEBUG("[FACE-FAST] Player {} direction {} -> coords ({}, {})",
                  m_Username, pendingDir, faceX, faceY);

        return true;  // Had an update
    }

    bool Player::ProcessFastMovement(int32_t& outPrimaryDir, int32_t& outSecondaryDir, bool& outMovementCompleted)
    {
        // Process timer-based movement (called from fast pulse thread at 50ms intervals)

        outMovementCompleted = false;
        outPrimaryDir = -1;
        outSecondaryDir = -1;

        // CAMERALOCK/CHASE CAMERA DIRECT MOVEMENT: respects run/walk state
        // Recalculate direction from face coordinates each tick to allow turning while moving
        if (m_CameraLockMoving) {
            auto now = std::chrono::steady_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
                now - m_CameraLockLastMoveTime).count();

            if (elapsed >= 600) {  // 600ms per tick (same for walk and run)
                m_CameraLockLastMoveTime = now;

                // Recalculate movement direction from current face coordinates
                // This allows turning while moving in chase camera mode
                int32_t playerX = GetPosition().GetX();
                int32_t playerY = GetPosition().GetY();
                int32_t centerX = playerX * 2 + 1;
                int32_t centerY = playerY * 2 + 1;
                int32_t faceDx = m_FaceX - centerX;
                int32_t faceDy = m_FaceY - centerY;

                double angle = std::atan2(static_cast<double>(faceDx), static_cast<double>(faceDy));
                double degrees = angle * 180.0 / 3.14159265358979;
                if (degrees < 0) degrees += 360.0;

                int32_t sector = static_cast<int32_t>((degrees + 22.5) / 45.0) % 8;
                static const int32_t SECTOR_TO_MOVEMENT[] = {1, 2, 4, 7, 6, 5, 3, 0};
                int32_t movementDir = SECTOR_TO_MOVEMENT[sector];

                // Update stored direction (for logging)
                m_CameraLockMoveDirection = movementDir;

                // Direction deltas for movement directions 0-7
                static const int32_t DIR_DX[] = {-1,  0,  1, -1, 1, -1, 0, 1};  // NW, N, NE, W, E, SW, S, SE
                static const int32_t DIR_DY[] = { 1,  1,  1,  0, 0, -1, -1, -1};

                int32_t dx = DIR_DX[movementDir];
                int32_t dy = DIR_DY[movementDir];

                bool isRunning = m_MovementHandler->IsRunning();

                // TODO: Collision check here

                // First step (walk or run)
                GetPosition().Move(dx, dy);
                outPrimaryDir = m_CameraLockMoveDirection;

                // Second step if running
                if (isRunning) {
                    // TODO: Collision check for second step
                    GetPosition().Move(dx, dy);
                    outSecondaryDir = m_CameraLockMoveDirection;
                }

                LOG_DEBUG("[CAMERALOCK-MOVE] Player {} dir={} run={} pos=({},{})",
                          m_Username, m_CameraLockMoveDirection, isRunning,
                          GetPosition().GetX(), GetPosition().GetY());

                // Check for map region change
                int32_t localX = GetPosition().GetLocalX(GetCurrentRegion());
                int32_t localY = GetPosition().GetLocalY(GetCurrentRegion());
                if (localX < 16 || localX >= 88 || localY < 16 || localY >= 88) {
                    UpdateMapRegion();
                }

                return true;
            }
            return false;  // Not time to move yet
        }

        // Normal movement (non-cameralock) - use waypoint queue
        m_MovementHandler->ProcessFast();
        outMovementCompleted = m_MovementHandler->HasMovementJustCompleted();

        if (m_MovementHandler->HasPendingMovement()) {
            outPrimaryDir = GetPrimaryDirection();
            outSecondaryDir = GetSecondaryDirection();
            SetPrimaryDirection(-1);
            SetSecondaryDirection(-1);

            if (outMovementCompleted) {
                SetFaceFromMovementDirection(m_MovementHandler->GetLastMovementDirection());
            }

            return true;
        }

        return false;
    }

    void Player::SetFaceFromMovementDirection(int32_t movementDir)
    {
        // Convert movement direction (0-7) to face coordinate
        // Movement directions: 0=NW, 1=N, 2=NE, 3=W, 4=E, 5=SW, 6=S, 7=SE
        // We need to set a face coordinate that makes the player face that direction

        int32_t playerX = GetPosition().GetX();
        int32_t playerY = GetPosition().GetY();

        // Direction deltas for movement directions 0-7
        static const int32_t DIR_X[] = {-1,  0,  1, -1, 1, -1, 0, 1};  // NW, N, NE, W, E, SW, S, SE
        static const int32_t DIR_Y[] = { 1,  1,  1,  0, 0, -1, -1, -1};

        int32_t dx = (movementDir >= 0 && movementDir < 8) ? DIR_X[movementDir] : 0;
        int32_t dy = (movementDir >= 0 && movementDir < 8) ? DIR_Y[movementDir] : 1;  // Default north

        // Face coordinate uses world coords * 2 + 1 for center, then offset
        constexpr int32_t FACE_DISTANCE = 512;
        int32_t faceX = playerX * 2 + 1 + dx * FACE_DISTANCE;
        int32_t faceY = playerY * 2 + 1 + dy * FACE_DISTANCE;

        SetFaceCoordinate(faceX, faceY);

        LOG_DEBUG("[FACE] Player {} facing movement direction {} -> coords ({}, {})",
                  m_Username, movementDir, faceX, faceY);
    }

    void Player::QueueFastPacket(int32_t opcode)
    {
        std::lock_guard<std::mutex> lock(m_FastPacketMutex);
        m_FastPacketQueue.push(opcode);
    }

    void Player::ProcessFastPackets()
    {
        // Currently unused - MoveControl (201) now handled via standard PacketManager
        // Kept for potential future fast-channel packets
        std::lock_guard<std::mutex> lock(m_FastPacketMutex);
        while (!m_FastPacketQueue.empty()) {
            m_FastPacketQueue.pop();
        }
    }

    void Player::Reset()
    {
        SetPrimaryDirection(-1);
        SetSecondaryDirection(-1);
        ClearUpdateFlags();
        m_CurrentChatMessage.reset();
        m_CombatHandler->Reset();
        m_AnimationId = -1;
        m_AnimationDelay = 0;
    }

    void Player::Login()
    {
        LOG_INFO("[LOGIN] Starting login sequence for player: {}", m_Username);

        LOG_INFO("[LOGIN] Updating map region...");
        UpdateMapRegion();

        LOG_INFO("[LOGIN] Setting update flag All");
        SetUpdateFlag(UpdateFlag::All);

        LOG_INFO("[LOGIN] Sending window pane...");
        StreamBuffer outStream1(16);
        InterfacePackets::SendWindowPane(outStream1, *GetClient()->GetEncryptor(), 548);
        GetClient()->Send(outStream1);
        StreamBuffer outStream2(16);
        InterfacePackets::SendComponentPosition(outStream2, *GetClient()->GetEncryptor(), 548, 101, 1000, 1000);
        GetClient()->Send(outStream2);

        LOG_INFO("[LOGIN] Sending welcome message...");
        StreamBuffer outStream3(256);
        PlayerPackets::SendPlayerMessage(outStream3, *GetClient()->GetEncryptor(), "Welcome to MetaSync 464.");
        GetClient()->Send(outStream3);

        LOG_INFO("[LOGIN] Sending interaction options...");
        StreamBuffer outStream4(16);
        PlayerPackets::SendInteractionOption(outStream4, *GetClient()->GetEncryptor(), "null", 1, true);
        GetClient()->Send(outStream4);
        StreamBuffer outStream5(16);
        PlayerPackets::SendInteractionOption(outStream5, *GetClient()->GetEncryptor(), "null", 2, false);
        GetClient()->Send(outStream5);
        StreamBuffer outStream6(16);
        PlayerPackets::SendInteractionOption(outStream6, *GetClient()->GetEncryptor(), "Follow", 3, false);
        GetClient()->Send(outStream6);
        StreamBuffer outStream7(16);
        PlayerPackets::SendInteractionOption(outStream7, *GetClient()->GetEncryptor(), "Trade with", 4, false);
        GetClient()->Send(outStream7);

        LOG_INFO("[LOGIN] Sending XP counter config...");
        StreamBuffer outStream8(16);
        WorldPackets::SendConfig(outStream8, *GetClient()->GetEncryptor(), 555, 0);
        GetClient()->Send(outStream8);

        LOG_INFO("[LOGIN] Sending skills...");
        for (int i = 0; i < 21; i++) {
            StreamBuffer skillStream(16);
            PlayerPackets::SendSkill(skillStream, *GetClient()->GetEncryptor(), i, 99, 13034431);
            GetClient()->Send(skillStream);
        }

        LOG_INFO("[LOGIN] Sending sidebar interfaces...");
        const std::vector<std::pair<int, int>> sidebars = {
            {77, 137}, {86, 92}, {87, 320}, {88, 274}, {89, 149},
            {90, 387}, {91, 271}, {92, 192}, {94, 550}, {95, 551},
            {96, 182}, {97, 261}, {98, 464}, {99, 239}
        };
        for (const auto& [menuId, form] : sidebars) {
            StreamBuffer sidebarStream(16);
            InterfacePackets::SendSidebarInterface(sidebarStream, *GetClient()->GetEncryptor(), menuId, form);
            GetClient()->Send(sidebarStream);
        }

        LOG_INFO("[LOGIN] Sending player configuration...");

        StreamBuffer outStream9(16);
        InterfacePackets::SendSidebarInterface(outStream9, *GetClient()->GetEncryptor(), 86, 92);
        GetClient()->Send(outStream9);

        LOG_INFO("[LOGIN] Sending inventory items...");
        std::vector<std::pair<int32_t, int32_t>> inventoryItems(28, {0, 0});
        StreamBuffer outStream10(inventoryItems.size() * 7 + 32);
        WorldPackets::SendUpdateItems(outStream10, *GetClient()->GetEncryptor(), 149, 0, 93, inventoryItems);
        GetClient()->Send(outStream10);

        LOG_INFO("[LOGIN] Sending equipment items...");
        std::vector<std::pair<int32_t, int32_t>> equipmentItems(14, {0, 0});
        StreamBuffer outStream11(equipmentItems.size() * 7 + 32);
        WorldPackets::SendUpdateItems(outStream11, *GetClient()->GetEncryptor(), 387, 28, 94, equipmentItems);
        GetClient()->Send(outStream11);

        LOG_INFO("[LOGIN] Sending bonus stats...");
        const std::vector<std::pair<int, std::string>> bonusStats = {
            {108, "Stab: +0"}, {109, "Slash: +0"}, {110, "Crush: +0"}, {111, "Magic: +0"},
            {112, "Range: +0"}, {114, "Stab: +0"}, {115, "Slash: +0"}, {116, "Crush: +0"},
            {117, "Magic: +0"}, {118, "Range: +0"}, {120, "Strength: +0"}, {121, "Prayer: +0"}
        };
        for (const auto& [childId, text] : bonusStats) {
            StreamBuffer bonusStream(text.size() + 16);
            InterfacePackets::SendString(bonusStream, *GetClient()->GetEncryptor(), 465, childId, text);
            GetClient()->Send(bonusStream);
        }

        LOG_INFO("[LOGIN] Login sequence completed for player: {}", m_Username);
    }

    void Player::SetAppearance(uint8_t gender, uint8_t head, uint8_t beard, uint8_t chest, uint8_t arms,
                                uint8_t hands, uint8_t legs, uint8_t feet, uint8_t hairColour, uint8_t torsoColour,
                                uint8_t legColour, uint8_t feetColour, uint8_t skinColour)
    {
        m_Gender = gender;

        m_Appearance[0] = chest;
        m_Appearance[1] = arms;
        m_Appearance[2] = legs;
        m_Appearance[3] = head;
        m_Appearance[4] = hands;
        m_Appearance[5] = feet;
        m_Appearance[6] = beard;

        m_Colors[0] = hairColour;
        m_Colors[1] = torsoColour;
        m_Colors[2] = legColour;
        m_Colors[3] = feetColour;
        m_Colors[4] = skinColour;

        LOG_INFO("[APPEARANCE] Updated appearance for player '{}' (gender={})", m_Username, gender);
    }

    void Player::RandomizeAppearance()
    {
        static std::random_device rd;
        static std::mt19937 gen(rd());

        // Gender: 0 = male, 1 = female
        std::uniform_int_distribution<> genderDist(0, 1);
        bool isFemale = genderDist(gen) == 1;

        // Appearance slots (identity kit IDs)
        // Male ranges: head 0-8, beard 10-17, chest 18-25, arms 26-31, hands 33-34, legs 36-40, feet 42-43
        // Female ranges: head 45-54, chest 56-60, arms 61-65, hands 67-68, legs 70-77, feet 79-80

        uint8_t head, beard, chest, arms, hands, legs, feet;

        if (isFemale) {
            std::uniform_int_distribution<> headDist(45, 54);
            std::uniform_int_distribution<> chestDist(56, 60);
            std::uniform_int_distribution<> armsDist(61, 65);
            std::uniform_int_distribution<> handsDist(67, 68);
            std::uniform_int_distribution<> legsDist(70, 77);
            std::uniform_int_distribution<> feetDist(79, 80);

            head = headDist(gen);
            beard = 0;  // No beard for female
            chest = chestDist(gen);
            arms = armsDist(gen);
            hands = handsDist(gen);
            legs = legsDist(gen);
            feet = feetDist(gen);
        } else {
            std::uniform_int_distribution<> headDist(0, 8);
            std::uniform_int_distribution<> beardDist(10, 17);
            std::uniform_int_distribution<> chestDist(18, 25);
            std::uniform_int_distribution<> armsDist(26, 31);
            std::uniform_int_distribution<> handsDist(33, 34);
            std::uniform_int_distribution<> legsDist(36, 40);
            std::uniform_int_distribution<> feetDist(42, 43);

            head = headDist(gen);
            beard = beardDist(gen);
            chest = chestDist(gen);
            arms = armsDist(gen);
            hands = handsDist(gen);
            legs = legsDist(gen);
            feet = feetDist(gen);
        }

        // Colors (0-29 for most, 0-7 for skin)
        std::uniform_int_distribution<> colorDist(0, 29);
        std::uniform_int_distribution<> skinDist(0, 7);

        uint8_t hairColor = colorDist(gen);
        uint8_t torsoColor = colorDist(gen);
        uint8_t legColor = colorDist(gen);
        uint8_t feetColor = colorDist(gen);
        uint8_t skinColor = skinDist(gen);

        // Apply the randomized appearance
        SetAppearance(isFemale ? 1 : 0, head, beard, chest, arms, hands, legs, feet,
                      hairColor, torsoColor, legColor, feetColor, skinColor);

        LOG_INFO("[APPEARANCE] Randomized appearance for new player '{}': gender={}, head={}, beard={}, chest={}, arms={}, hands={}, legs={}, feet={}",
                 m_Username, isFemale ? "female" : "male", head, beard, chest, arms, hands, legs, feet);
    }

    void Player::UpdateMapRegion()
    {
        m_CurrentRegion = GetPosition();
        SetUpdateFlag(UpdateFlag::NeedsPlacement);
        
        auto client = GetClient();
        if (!client) return;
        
        int32_t localX = GetPosition().GetX() - 8 * (GetPosition().GetRegionX() - 6);
        int32_t localY = GetPosition().GetY() - 8 * (GetPosition().GetRegionY() - 6);
        
        StreamBuffer outStream(256);
        WorldPackets::SendMapRegion(outStream, *client->GetEncryptor(), 
            GetPosition().GetRegionX(), GetPosition().GetRegionY(),
            localX, localY, GetPosition().GetZ());
        client->Send(outStream);
    }

    void Player::ProcessCommand(const std::string& commandLine)
    {

        std::string trimmed = commandLine;

        size_t start = trimmed.find_first_not_of(" \t");
        if (start == std::string::npos) {
            SendMessage("The command you have entered does not exist.");
            return;
        }
        trimmed = trimmed.substr(start);

        size_t end = trimmed.find_last_not_of(" \t");
        if (end != std::string::npos) {
            trimmed = trimmed.substr(0, end + 1);
        }

        if (trimmed.empty()) {
            SendMessage("The command you have entered does not exist.");
            return;
        }

        std::vector<std::string> parts;
        std::stringstream ss(trimmed);
        std::string token;

        while (std::getline(ss, token, ' ')) {
            if (!token.empty()) {
                parts.push_back(token);
            }
        }

        if (parts.empty()) {
            SendMessage("The command you have entered does not exist.");
            return;
        }

        std::string command = parts[0];
        std::vector<std::string> args(parts.begin() + 1, parts.end());

        LOG_INFO("[COMMAND] Processing command: '{}' with {} args", command, args.size());

        if (command == "hd" || command == "ld") {
            return;
        }

        if (command == "tele") {
            if (args.size() == 2 || args.size() == 3) {
                try {
                    int32_t x = std::stoi(args[0]);
                    int32_t y = std::stoi(args[1]);
                    int32_t z = GetPosition().GetZ();

                    if (args.size() == 3) {
                        z = std::stoi(args[2]);
                    }

                    GetMovementHandler().SetTeleportTarget(Position(x, y, z));
                    SendMessage("Teleporting to: " + std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z));
                    LOG_INFO("[COMMAND] Player {} teleporting to ({}, {}, {})", GetUsername(), x, y, z);

                } catch (const std::exception& e) {
                    SendMessage("Syntax is ::tele [x] [y] [z].");
                    LOG_ERROR("[COMMAND] Error parsing teleport coordinates: {}", e.what());
                }
            } else {
                SendMessage("Syntax is ::tele [x] [y] [z].");
            }
            return;
        }

        if (command == "designinfo") {
            SendMessage("=== Character Design Help ===");
            SendMessage("Usage: ::design <gender> <head> <beard> <chest> <arms> <hands> <legs> <feet> <hairColor> <torsoColor> <legColor> <feetColor> <skinColor>");
            SendMessage("Gender: 0=Male, 1=Female");
            SendMessage("Body parts: 0-50 (varies by part)");
            SendMessage("Colors: 0-24 for most colors");
            SendMessage("Example: ::design 0 3 10 18 26 33 36 42 7 8 9 5 0");
            SendMessage("(Male, head=3, beard=10, default body, default colors)");
            return;
        }

        if (command == "design") {
            if (args.size() != 13) {
                SendMessage("Usage: ::design <gender> <head> <beard> <chest> <arms> <hands> <legs> <feet> <hairColor> <torsoColor> <legColor> <feetColor> <skinColor>");
                SendMessage("Type ::designinfo for detailed help.");
                return;
            }

            try {
                uint8_t gender = std::stoi(args[0]);
                uint8_t head = std::stoi(args[1]);
                uint8_t beard = std::stoi(args[2]);
                uint8_t chest = std::stoi(args[3]);
                uint8_t arms = std::stoi(args[4]);
                uint8_t hands = std::stoi(args[5]);
                uint8_t legs = std::stoi(args[6]);
                uint8_t feet = std::stoi(args[7]);
                uint8_t hairColour = std::stoi(args[8]);
                uint8_t torsoColour = std::stoi(args[9]);
                uint8_t legColour = std::stoi(args[10]);
                uint8_t feetColour = std::stoi(args[11]);
                uint8_t skinColour = std::stoi(args[12]);

                SetAppearance(1, head, beard, chest, arms, hands, legs, feet,
                             hairColour, torsoColour, legColour, feetColour, skinColour);
                SetUpdateFlag(UpdateFlag::All);

                SendMessage("Appearance updated successfully!");
                LOG_INFO("[COMMAND] Player {} updated appearance: gender={}, head={}, beard={}, chest={}, arms={}, hands={}, legs={}, feet={}",
                    GetUsername(), gender, head, beard, chest, arms, hands, legs, feet);
                LOG_INFO("[COMMAND] Colors: hair={}, torso={}, leg={}, feet={}, skin={}",
                    hairColour, torsoColour, legColour, feetColour, skinColour);

            } catch (const std::exception& e) {
                SendMessage("Invalid arguments. All values must be numbers.");
                SendMessage("Type ::designinfo for help.");
                LOG_ERROR("[COMMAND] Error parsing design arguments: {}", e.what());
            }
            return;
        }

        if (command == "pos") {
            const Position& pos = GetPosition();
            SendMessage("Your current position: " + std::to_string(pos.GetX()) + ", " +
                       std::to_string(pos.GetY()) + ", " + std::to_string(pos.GetZ()));
            return;
        }

        if (command == "logout") {
            SendMessage("Logging out...");
            LOG_INFO("[COMMAND] Player {} requesting logout", GetUsername());
            if (auto client = GetClient()) {
                StreamBuffer outStream(8);
                SystemPackets::SendLogout(outStream, *client->GetEncryptor());
                client->Send(outStream);
                client->RequestDisconnect();
            }
            return;
        }

        if (command == "intf") {
            if (args.size() != 1) {
                SendMessage("Syntax is ::intf [id].");
                return;
            }

            try {
                int32_t interfaceId = std::stoi(args[0]);
                if (auto client = GetClient()) {
                    StreamBuffer outStream(2048);
                    InterfacePackets::SendInterface(outStream, *client->GetEncryptor(), interfaceId);
                    client->Send(outStream);
                    SendMessage("Opened interface: " + std::to_string(interfaceId));
                }
            } catch (const std::exception& e) {
                SendMessage("Syntax is ::intf [id].");
                LOG_ERROR("[COMMAND] Error parsing interface ID: {}", e.what());
            }
            return;
        }

        if (command == "close") {
            if (auto client = GetClient()) {
                StreamBuffer outStream(2048);
                InterfacePackets::CloseInterface(outStream, *client->GetEncryptor());
                client->Send(outStream);
            }
            return;
        }

        if (command == "anim") {
            if (args.size() < 1 || args.size() > 2) {
                SendMessage("Syntax is ::anim [id] [delay].");
                return;
            }

            try {
                int32_t animationId = std::stoi(args[0]);
                int32_t delay = 0;
                if (args.size() == 2) {
                    delay = std::stoi(args[1]);
                }
                PlayAnimation(animationId, delay);
            } catch (const std::exception& e) {
                SendMessage("Syntax is ::anim [id] [delay].");
            }
            return;
        }

        // Toggle 530 player animation mode (higher vertex count skeleton animations)
        if (command == "530player") {
            m_Use530Animations = true;
            SetUpdateFlag(UpdateFlag::Appearance);
            SendMessage("530 player animations ENABLED.");
            SendMessage("Using animation IDs: idle=10000, walk=10021, run=10022");
            LOG_INFO("[COMMAND] Player {} enabled 530 animation mode", GetUsername());
            return;
        }

        // Toggle back to 464 player animation mode
        if (command == "464player") {
            m_Use530Animations = false;
            SetUpdateFlag(UpdateFlag::Appearance);
            SendMessage("464 player animations ENABLED.");
            SendMessage("Using animation IDs: idle=808, walk=819, run=824");
            LOG_INFO("[COMMAND] Player {} disabled 530 animation mode (back to 464)", GetUsername());
            return;
        }

        // Toggle camera-locked facing mode
        if (command == "cameralock") {
            m_CameraLockFacing = !m_CameraLockFacing;
            // Disable chase camera if enabling camera lock (mutually exclusive)
            if (m_CameraLockFacing) {
                m_ChaseCameraMode = false;
                SendMessage("Camera-locked facing ENABLED.");
                SendMessage("Player will face camera direction when idle.");
            } else {
                SendMessage("Camera-locked facing DISABLED.");
            }
            LOG_INFO("[COMMAND] Player {} {} camera-lock facing", GetUsername(), m_CameraLockFacing ? "enabled" : "disabled");
            return;
        }

        // Toggle chase camera mode (camera orbits behind player)
        if (command == "chasecamera") {
            m_ChaseCameraMode = !m_ChaseCameraMode;
            // Disable camera lock if enabling chase camera (mutually exclusive)
            if (m_ChaseCameraMode) {
                m_CameraLockFacing = false;
                SendMessage("Chase camera ENABLED.");
                SendMessage("Camera orbits behind player. Left/Right rotates player.");
            } else {
                SendMessage("Chase camera DISABLED.");
            }
            LOG_INFO("[COMMAND] Player {} {} chase camera", GetUsername(), m_ChaseCameraMode ? "enabled" : "disabled");
            return;
        }

        // SendMessage("The command you have entered does not exist.");
        // LOG_WARN("[COMMAND] Unknown command: '{}'", command);
    }

    void Player::PlayAnimation(int32_t animationId, int32_t delay)
    {
        m_AnimationId = animationId;
        m_AnimationDelay = delay;
        SetUpdateFlag(UpdateFlag::Animation);
    }

    void Player::QueueFaceDirection(int32_t direction)
    {
        // Add face direction to the queue (thread-safe)
        // This is called from the network thread, will be processed during next fast tick
        std::lock_guard<std::mutex> lock(m_FaceDirectionMutex);
        // Clear any existing queue when a new single direction is set
        // (user changed direction, don't complete old turn animation)
        while (!m_PendingFaceDirections.empty()) {
            m_PendingFaceDirections.pop();
        }
        m_PendingFaceDirections.push(direction);
    }

    void Player::QueueMultipleFaceDirections(const std::vector<int32_t>& directions)
    {
        // Queue multiple face directions for smooth turn animation (thread-safe)
        // Each direction will be processed one per fast tick (50ms)
        std::lock_guard<std::mutex> lock(m_FaceDirectionMutex);
        // Clear existing queue
        while (!m_PendingFaceDirections.empty()) {
            m_PendingFaceDirections.pop();
        }
        // Add all new directions
        for (int32_t dir : directions) {
            m_PendingFaceDirections.push(dir);
        }
    }

    void Player::SendMessage(const std::string& message)
    {
        if (auto client = GetClient()) {
            StreamBuffer outStream(message.size() + 3);
            PlayerPackets::SendPlayerMessage(outStream, *client->GetEncryptor(), message);
            client->Send(outStream);
        }
    }

    int32_t Player::GetMaxHitpoints() const {
        return m_SkillHandler->GetEffectiveLevel(Skill::HITPOINTS);
    }

    void Player::SetHitpoints(int32_t hp) {
        int32_t maxHp = GetMaxHitpoints();
        m_Hitpoints = std::clamp(hp, 0, maxHp);

        // TODO: Send HP update packet to client
        // TODO: Check for death

        if (m_Hitpoints <= 0) {
            // TODO: Trigger death event
            LOG_INFO("[COMBAT] Player {} has died", m_Username);
        }
    }

    void Player::Damage(int32_t amount) {
        if (amount <= 0) return;

        m_Hitpoints -= amount;
        if (m_Hitpoints < 0) {
            m_Hitpoints = 0;
        }

        LOG_INFO("[COMBAT] Player {} took {} damage (HP: {})", m_Username, amount, m_Hitpoints);

        // TODO: Send HP update packet
        // TODO: Check for death

        if (m_Hitpoints <= 0) {
            // TODO: Trigger death event
            LOG_INFO("[COMBAT] Player {} has died", m_Username);
        }
    }

    void Player::Heal(int32_t amount) {
        if (amount <= 0) return;

        int32_t oldHp = m_Hitpoints;
        m_Hitpoints = std::min(m_Hitpoints + amount, GetMaxHitpoints());

        int32_t actualHeal = m_Hitpoints - oldHp;
        if (actualHeal > 0) {
            LOG_INFO("[COMBAT] Player {} healed {} HP (HP: {})", m_Username, actualHeal, m_Hitpoints);
        }

        // TODO: Send HP update packet
    }

}
