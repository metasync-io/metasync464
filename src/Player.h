#pragma once
#include "Client.h"
#include "Position.h"
#include "UpdateFlags.h"
#include "MovementHandler.h"

namespace Skeleton {

    class Player
    {
    public:
        Player(const std::string& username, std::weak_ptr<Client> client);

        const std::string& GetUsername() const;
        std::shared_ptr<Client> GetClient() const { return m_Client.lock(); }
        const Position& GetPosition() const { return m_Position; }
        Position& GetPosition() { return m_Position; }
        const std::array<uint32_t, 5>& GetColors() const { return m_Colors; }
        const std::array<uint32_t, 14>& GetEquipment() const { return m_Equipment; }
        const std::array<uint32_t, 7>& GetAppearance() const { return m_Appearance; }
        const uint32_t GetPrimaryDirection() const { return m_PrimaryDirection; }
        const uint32_t GetSecondaryDirection() const { return m_SecondaryDirection; }
        const uint32_t GetNPCCosplayId() const { return m_NPCCosplayId; }

        void SetPrimaryDirection(int32_t primaryDirection) { m_PrimaryDirection = primaryDirection; }
        void SetSecondaryDirection(int32_t secondaryDirection) { m_SecondaryDirection = secondaryDirection; }

        void SetIndex(int32_t index) { m_Index = index; }
        const int32_t GetIndex() const { return m_Index; }

        const Position& GetCurrentRegion() const { return m_CurrentRegion; }
        void SetCurrentRegion(Position& currentRegion) { currentRegion = m_CurrentRegion; }

        MovementHandler& GetMovementHandler() { return *m_MovementHandler; }
        const MovementHandler& GetMovementHandler() const { return *m_MovementHandler; }

        std::list<std::shared_ptr<Player>>& GetLocalPlayers() { return m_LocalPlayers; }

        void Logout();
        void Login();

        void Process();
        void Reset();

        void UpdateMapRegion();

        void SetUpdateFlag(UpdateFlag flag) { m_UpdateFlags |= flag; }
        bool NeedsUpdate(UpdateFlag flag) const { return hasFlag(m_UpdateFlags, flag); }
        void ClearUpdateFlags() { m_UpdateFlags = UpdateFlag::None;}
        bool NeedsUpdate() const { return static_cast<uint32_t>(m_UpdateFlags) != 0; }
        UpdateFlag GetUpdateFlags() const { return m_UpdateFlags; }

    private:
        std::array<uint32_t, 14> m_Equipment = { };
        std::array<uint32_t, 5> m_Colors = { 7, 8, 9, 5, 0 };
        std::array<uint32_t, 7> m_Appearance = { 18, 26, 36, 0, 33, 42, 10 };
        int32_t m_PrimaryDirection = -1;
        int32_t m_SecondaryDirection = -1;
        uint32_t m_NPCCosplayId = -1;
        std::string m_Username = "";
        std::weak_ptr<Client> m_Client; // server accepts/owns sessions, player can exist in game without client for short period
        Position m_Position;
        Position m_CurrentRegion;
        UpdateFlag m_UpdateFlags = UpdateFlag::None;
        std::unique_ptr<MovementHandler> m_MovementHandler;
        int32_t m_Index = -1;

        std::list<std::shared_ptr<Player>> m_LocalPlayers;
    };

}