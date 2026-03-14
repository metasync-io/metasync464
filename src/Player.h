#pragma once
#include "Client.h"
#include "Position.h"
#include "UpdateFlags.h"
#include "MovementHandler.h"
#include "ActionQueue.h"
#include "Equipment/EquipmentHandler.h"
#include "Combat/HitQueue.h"
#include "Combat/CombatHandler.h"
#include "Skills/SkillHandler.h"
#include "Items/Inventory.h"
#include <queue>
#include <atomic>
#include <cmath>
#include <mutex>

namespace Skeleton {

    struct ChatMessage {
        int32_t colour;
        int32_t numChars;
        std::string chatText;
        int32_t effect;
        std::vector<uint8_t> packed;

        ChatMessage(int32_t c, int32_t n, const std::string& t, int32_t e)
            : colour(c), numChars(n), chatText(t), effect(e) {}
    };

    class Player
    {
    public:

        Player(const std::string& username, std::weak_ptr<Client> client);
        ~Player();

        const std::string& GetUsername() const;

        std::shared_ptr<Client> GetClient() const { return m_Client.lock(); }

        const Position& GetPosition() const { return m_Position; }
        Position& GetPosition() { return m_Position; }

        const std::array<uint32_t, 5>& GetColors() const { return m_Colors; }
        const std::array<uint32_t, 14>& GetEquipment() const { return m_Equipment; }
        const std::array<uint32_t, 7>& GetAppearance() const { return m_Appearance; }

        uint8_t GetGender() const { return m_Gender; }
        void SetGender(uint8_t gender) { m_Gender = gender; }
        void SetColors(const std::array<uint32_t, 5>& colors) { m_Colors = colors; }
        void SetAppearanceArray(const std::array<uint32_t, 7>& appearance) { m_Appearance = appearance; }

        const uint32_t GetPrimaryDirection() const { return m_PrimaryDirection; }
        const uint32_t GetSecondaryDirection() const { return m_SecondaryDirection; }
        void SetPrimaryDirection(int32_t primaryDirection) { m_PrimaryDirection = primaryDirection; }
        void SetSecondaryDirection(int32_t secondaryDirection) { m_SecondaryDirection = secondaryDirection; }

        const uint32_t GetNPCCosplayId() const { return m_NPCCosplayId; }

        void SetIndex(int32_t index) { m_Index = index; }
        void setIndex(int32_t index) { m_Index = index; }
        const int32_t GetIndex() const { return m_Index; }
        int32_t getIndex() const { return m_Index; }

        const Position& GetCurrentRegion() const { return m_CurrentRegion; }
        void SetCurrentRegion(Position& currentRegion) { currentRegion = m_CurrentRegion; }

        void SetAppearance(uint8_t gender, uint8_t head, uint8_t beard, uint8_t chest, uint8_t arms,
                          uint8_t hands, uint8_t legs, uint8_t feet, uint8_t hairColour, uint8_t torsoColour,
                          uint8_t legColour, uint8_t feetColour, uint8_t skinColour);

        void RandomizeAppearance();

        bool IsNewPlayer() const { return m_IsNewPlayer; }
        void SetNewPlayer(bool isNew) { m_IsNewPlayer = isNew; }

        MovementHandler& GetMovementHandler() { return *m_MovementHandler; }
        const MovementHandler& GetMovementHandler() const { return *m_MovementHandler; }

        ActionQueue& GetActionQueue() { return m_ActionQueue; }
        const ActionQueue& GetActionQueue() const { return m_ActionQueue; }

        EquipmentHandler& GetEquipmentHandler() { return *m_EquipmentHandler; }
        const EquipmentHandler& GetEquipmentHandler() const { return *m_EquipmentHandler; }

        CombatHandler& GetCombatHandler() { return *m_CombatHandler; }
        const CombatHandler& GetCombatHandler() const { return *m_CombatHandler; }

        HitQueue& GetHitQueue() { return m_HitQueue; }
        const HitQueue& GetHitQueue() const { return m_HitQueue; }

        SkillHandler& GetSkills() { return *m_SkillHandler; }
        const SkillHandler& GetSkills() const { return *m_SkillHandler; }

        Inventory& GetInventory() { return *m_Inventory; }
        const Inventory& GetInventory() const { return *m_Inventory; }

        std::list<std::shared_ptr<Player>>& GetLocalPlayers() { return m_LocalPlayers; }

        void Logout();

        void Login();

        void Process();

        void Reset();

        void UpdateMapRegion();

        void ProcessCommand(const std::string& commandLine);

        void SendMessage(const std::string& message);

        void SetUpdateFlag(UpdateFlag flag) { m_UpdateFlags |= flag; }
        bool NeedsUpdate(UpdateFlag flag) const { return hasFlag(m_UpdateFlags, flag); }
        void ClearUpdateFlags() { m_UpdateFlags = UpdateFlag::None;}
        bool NeedsUpdate() const { return static_cast<uint32_t>(m_UpdateFlags) != 0; }
        UpdateFlag GetUpdateFlags() const { return m_UpdateFlags; }

        std::queue<ChatMessage>& GetChatMessageQueue() { return m_ChatMessageQueue; }
        ChatMessage* GetCurrentChatMessage() { return m_CurrentChatMessage.get(); }
        void SetCurrentChatMessage(std::unique_ptr<ChatMessage> msg) { m_CurrentChatMessage = std::move(msg); }

        void PlayAnimation(int32_t animationId, int32_t delay = 0);
        int32_t GetAnimationId() const { return m_AnimationId; }
        int32_t GetAnimationDelay() const { return m_AnimationDelay; }

        // 530 animation mode - uses higher vertex count skeleton animations
        bool Get530AnimationMode() const { return m_Use530Animations; }
        void Set530AnimationMode(bool enabled) { m_Use530Animations = enabled; }

        // Face coordinate for direction updates
        void SetFaceCoordinate(int32_t x, int32_t y) { m_FaceX = x; m_FaceY = y; }
        int32_t GetFaceX() const { return m_FaceX; }
        int32_t GetFaceY() const { return m_FaceY; }

        // Queue face direction update (called from packet handler on network thread)
        void QueueFaceDirection(int32_t direction);

        // Queue multiple face directions for smooth turn animation (e.g., 180° turn)
        void QueueMultipleFaceDirections(const std::vector<int32_t>& directions);

        // Process fast face update (called from fast pulse thread)
        // Returns true if there was a pending update that was processed
        bool ProcessFastFaceUpdate();

        // Process fast movement (called from fast pulse thread)
        // Returns true if movement was processed, and outputs the directions
        // Also sets outMovementCompleted if this was the final step
        bool ProcessFastMovement(int32_t& outPrimaryDir, int32_t& outSecondaryDir, bool& outMovementCompleted);

        // Set face direction based on movement direction (0-7) and flag for update
        void SetFaceFromMovementDirection(int32_t movementDir);

        // Camera-lock facing mode
        bool GetCameraLockMode() const { return m_CameraLockFacing; }
        void SetCameraLockMode(bool enabled) { m_CameraLockFacing = enabled; }

        // Chase camera mode (camera orbits behind player)
        bool GetChaseCameraMode() const { return m_ChaseCameraMode; }
        void SetChaseCameraMode(bool enabled) { m_ChaseCameraMode = enabled; }

        // Check if any camera mode that uses continuous movement is active
        bool IsAnyCameraMoveMode() const { return m_CameraLockFacing || m_ChaseCameraMode; }

        // Camera-lock continuous movement (while Up key held)
        bool IsCameraLockMoving() const { return m_CameraLockMoving; }
        void SetCameraLockMoving(bool moving) { m_CameraLockMoving = moving; }
        int32_t GetCameraLockMoveDirection() const { return m_CameraLockMoveDirection; }
        void SetCameraLockMoveDirection(int32_t dir) { m_CameraLockMoveDirection = dir; }

        // Fine position (sub-tile precision for smooth movement sync)
        // High precision = 8x game units (1024 subdivisions per tile)
        void SetFinePosition(int32_t fineX, int32_t fineZ) {
            m_FineX = fineX;
            m_FineZ = fineZ;
            m_HasFinePosition = true;
        }
        int32_t GetFineX() const { return m_FineX; }
        int32_t GetFineZ() const { return m_FineZ; }
        bool HasFinePosition() const { return m_HasFinePosition; }
        void ClearFinePosition() { m_HasFinePosition = false; }

        // Fast packet queue for opcodes that need low-latency processing (201, 202)
        void QueueFastPacket(int32_t opcode);
        void ProcessFastPackets();

        // Hitpoints management
        int32_t GetHitpoints() const { return m_Hitpoints; }
        int32_t GetMaxHitpoints() const;
        void SetHitpoints(int32_t hp);
        void Damage(int32_t amount);
        void Heal(int32_t amount);
        bool IsDead() const { return m_Hitpoints <= 0; }

    private:

        std::array<uint32_t, 14> m_Equipment = { };

        std::array<uint32_t, 5> m_Colors = { 7, 8, 9, 5, 0 };

        std::array<uint32_t, 7> m_Appearance = { 18, 26, 36, 0, 33, 42, 10 };

        uint8_t m_Gender = 0;  // 0 = male, 1 = female

        int32_t m_PrimaryDirection = -1;
        int32_t m_SecondaryDirection = -1;

        uint32_t m_NPCCosplayId = -1;

        std::string m_Username = "";

        std::weak_ptr<Client> m_Client;

        Position m_Position;

        Position m_CurrentRegion;

        UpdateFlag m_UpdateFlags = UpdateFlag::None;

        std::unique_ptr<MovementHandler> m_MovementHandler;

        ActionQueue m_ActionQueue;

        std::unique_ptr<EquipmentHandler> m_EquipmentHandler;

        std::unique_ptr<CombatHandler> m_CombatHandler;

        std::unique_ptr<SkillHandler> m_SkillHandler;

        std::unique_ptr<Inventory> m_Inventory;

        HitQueue m_HitQueue;

        int32_t m_Index = -1;

        std::list<std::shared_ptr<Player>> m_LocalPlayers;

        std::queue<ChatMessage> m_ChatMessageQueue;

        std::unique_ptr<ChatMessage> m_CurrentChatMessage;

        bool m_IsNewPlayer = false;

        int32_t m_AnimationId = -1;
        int32_t m_AnimationDelay = 0;

        int32_t m_Hitpoints = 10; // Current HP

        bool m_Use530Animations = false; // Use 530 revision skeleton animations (higher vertex count)

        // Face coordinate for direction updates
        int32_t m_FaceX = 0;
        int32_t m_FaceY = 0;

        // Pending face directions from packets (thread-safe queue)
        std::queue<int32_t> m_PendingFaceDirections;
        std::mutex m_FaceDirectionMutex;

        // Camera-lock facing mode
        bool m_CameraLockFacing = false;

        // Chase camera mode
        bool m_ChaseCameraMode = false;

        // Camera-lock continuous movement
        bool m_CameraLockMoving = false;
        int32_t m_CameraLockMoveDirection = -1;  // 0-7 direction, -1 = none
        std::chrono::steady_clock::time_point m_CameraLockLastMoveTime;

        // Fine position (sub-tile precision, 8x game units)
        int32_t m_FineX = 0;
        int32_t m_FineZ = 0;
        bool m_HasFinePosition = false;

        // Fast packet queue (for low-latency processing on fast pulse thread)
        std::queue<int32_t> m_FastPacketQueue;
        std::mutex m_FastPacketMutex;
    };

}
