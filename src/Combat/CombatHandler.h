#pragma once

#include <memory>
#include <cstdint>
#include "CombatState.h"

namespace Skeleton {

    class Player;
    class Mob;
    class CombatAction;

    class CombatHandler {
    public:
        CombatHandler(Player& player);
        ~CombatHandler();

        // Called from Player::Process()
        void Process();

        // Called from Player::Reset()
        void Reset();

        // Combat control
        void BeginCombat(std::weak_ptr<Player> target);
        void EndCombat();
        bool IsInCombat() const;

        // State accessors
        std::weak_ptr<Player> GetTarget() const { return m_Target; }
        int32_t GetAttackTimer() const { return m_AttackTimer; }
        void SetAttackTimer(int32_t timer) { m_AttackTimer = timer; }
        void DecrementAttackTimer();
        CombatType GetCombatType() const { return m_CombatType; }
        void SetCombatType(CombatType type) { m_CombatType = type; }

        // Attack style
        AttackStyle GetAttackStyle() const { return m_AttackStyle; }
        void SetAttackStyle(AttackStyle style) { m_AttackStyle = style; }

        // Special attack
        int32_t GetSpecialEnergy() const { return m_SpecialEnergy; }
        void SetSpecialEnergy(int32_t energy);
        bool CanUseSpecial(int32_t cost) const;
        void ConsumeSpecialEnergy(int32_t cost);
        bool IsSpecialEnabled() const { return m_SpecialEnabled; }
        void SetSpecialEnabled(bool enabled) { m_SpecialEnabled = enabled; }

        // Combat tracking
        int64_t GetLastHitTimestamp() const { return m_LastHitTimestamp; }
        void RefreshLastHit();
        int64_t GetLastAttackTimestamp() const { return m_LastAttackTimestamp; }
        void RefreshLastAttack();

        // Auto-retaliate
        bool IsAutoRetaliateEnabled() const { return m_AutoRetaliate; }
        void SetAutoRetaliate(bool enabled) { m_AutoRetaliate = enabled; }

        // Magic spell selection
        void SetSelectedSpell(int32_t spellId);
        int32_t GetSelectedSpell() const { return m_SelectedSpell; }
        void ClearSelectedSpell() { m_SelectedSpell = -1; }

    private:
        Player& m_Player;
        std::weak_ptr<Player> m_Target;
        int32_t m_AttackTimer = 0;
        int64_t m_LastHitTimestamp = 0;
        int64_t m_LastAttackTimestamp = 0;
        CombatType m_CombatType = CombatType::MELEE;
        AttackStyle m_AttackStyle = AttackStyle::MELEE_ACCURATE;
        int32_t m_SpecialEnergy = 100;
        bool m_SpecialEnabled = false;
        bool m_AutoRetaliate = true;
        int32_t m_SelectedSpell = -1;
        int64_t m_LastSpecialRegenTick = 0;
        std::unique_ptr<CombatAction> m_CurrentAction;

        // Helper methods
        void ProcessCombat();
        void DetermineCombatStyle();
        void RegenerateSpecialEnergy();
    };

}
