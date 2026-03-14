#include "CombatHandler.h"
#include "../Player.h"
#include "CombatAction.h"
#include "CombatConstants.h"
#include "MeleeAction.h"
#include "RangedAction.h"
#include "MagicAction.h"
#include "../Items/WeaponType.h"
#include <chrono>

namespace Skeleton {

    CombatHandler::CombatHandler(Player& player)
        : m_Player(player) {
    }

    CombatHandler::~CombatHandler() = default;

    void CombatHandler::Process() {
        // Decrement attack timer
        if (m_AttackTimer > 0) {
            m_AttackTimer--;
        }

        // Regenerate special energy
        RegenerateSpecialEnergy();

        // Check if we have a valid target
        auto target = m_Target.lock();
        if (!target) {
            return;
        }

        // TODO: Check if target is still valid (not dead, in range, etc.)

        // Process combat action if attack timer is ready
        if (m_AttackTimer <= 0) {
            ProcessCombat();
        }
    }

    void CombatHandler::Reset() {
        m_AttackTimer = 0;
    }

    void CombatHandler::BeginCombat(std::weak_ptr<Player> target) {
        m_Target = target;
        DetermineCombatStyle();

        // Create appropriate combat action based on combat type
        if (!m_CurrentAction || m_CurrentAction.get() == nullptr) {
            switch (m_CombatType) {
                case CombatType::MELEE:
                    m_CurrentAction = std::make_unique<MeleeAction>();
                    break;
                case CombatType::RANGED:
                    m_CurrentAction = std::make_unique<RangedAction>();
                    break;
                case CombatType::MAGIC:
                    m_CurrentAction = std::make_unique<MagicAction>();
                    break;
                default:
                    m_CurrentAction = std::make_unique<MeleeAction>();
                    break;
            }
        }

        RefreshLastAttack();
    }

    void CombatHandler::EndCombat() {
        m_Target.reset();
        m_AttackTimer = 0;
        m_CurrentAction.reset();
    }

    bool CombatHandler::IsInCombat() const {
        return !m_Target.expired();
    }

    void CombatHandler::DecrementAttackTimer() {
        if (m_AttackTimer > 0) {
            m_AttackTimer--;
        }
    }

    void CombatHandler::SetSpecialEnergy(int32_t energy) {
        m_SpecialEnergy = std::clamp(energy, 0, CombatConstants::MAX_SPECIAL_ENERGY);
    }

    bool CombatHandler::CanUseSpecial(int32_t cost) const {
        return m_SpecialEnergy >= cost;
    }

    void CombatHandler::ConsumeSpecialEnergy(int32_t cost) {
        m_SpecialEnergy = std::max(0, m_SpecialEnergy - cost);
        // TODO: Set update flag for special energy
    }

    void CombatHandler::RefreshLastHit() {
        auto now = std::chrono::steady_clock::now();
        m_LastHitTimestamp = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();
    }

    void CombatHandler::RefreshLastAttack() {
        auto now = std::chrono::steady_clock::now();
        m_LastAttackTimestamp = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();
    }

    void CombatHandler::SetSelectedSpell(int32_t spellId) {
        m_SelectedSpell = spellId;

        // If we have a MagicAction, set the spell on it
        if (m_CurrentAction && m_CombatType == CombatType::MAGIC) {
            MagicAction* magicAction = dynamic_cast<MagicAction*>(m_CurrentAction.get());
            if (magicAction) {
                magicAction->SetSpell(spellId);
            }
        }

        // Auto-switch to magic combat type when spell is selected
        m_CombatType = CombatType::MAGIC;
    }

    void CombatHandler::RegenerateSpecialEnergy() {
        // Special energy regenerates at 10% per minute (1% every 6 seconds)
        // In game ticks: 1% every 10 ticks (600ms tick rate = 6 seconds for 10 ticks)

        if (m_SpecialEnergy >= CombatConstants::MAX_SPECIAL_ENERGY) {
            return; // Already at max
        }

        auto now = std::chrono::steady_clock::now();
        int64_t currentTick = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count() / 600;

        if (m_LastSpecialRegenTick == 0) {
            m_LastSpecialRegenTick = currentTick;
            return;
        }

        int64_t ticksPassed = currentTick - m_LastSpecialRegenTick;

        // Regenerate 1% every 10 ticks
        if (ticksPassed >= 10) {
            int32_t regenAmount = static_cast<int32_t>(ticksPassed / 10);
            m_SpecialEnergy = std::min(m_SpecialEnergy + regenAmount, CombatConstants::MAX_SPECIAL_ENERGY);
            m_LastSpecialRegenTick = currentTick;

            // TODO: Set update flag for special energy
        }
    }

    void CombatHandler::ProcessCombat() {
        auto target = m_Target.lock();
        if (!target || !m_CurrentAction) {
            return;
        }

        // Check if we can attack
        if (!m_CurrentAction->CanAttack(m_Player, *target)) {
            EndCombat();
            return;
        }

        // Check if within radius
        if (!m_CurrentAction->IsWithinRadius(m_Player, *target)) {
            // TODO: Move closer to target
            return;
        }

        // Execute attack
        bool isSpecial = m_SpecialEnabled;
        m_CurrentAction->Begin(m_Player, *target, isSpecial);

        // Reset special after use
        if (isSpecial) {
            m_SpecialEnabled = false;
        }

        // TODO: Set attack timer based on weapon speed
        // For now, use default
        m_AttackTimer = CombatConstants::DEFAULT_ATTACK_SPEED;

        RefreshLastAttack();
    }

    void CombatHandler::DetermineCombatStyle() {
        // Check equipped weapon to determine combat style
        // TODO: Get weapon from equipment handler
        // For now, we'll need to implement this properly when equipment integration is complete

        // Default to melee
        m_CombatType = CombatType::MELEE;

        // TODO: Once equipment handler is fully integrated:
        // int32_t weaponId = m_Player.GetEquipmentHandler().GetItemInSlot(EquipmentSlot::WEAPON);
        // WeaponDefinition weaponDef = GetWeaponDefinition(weaponId);
        //
        // switch (weaponDef.m_WeaponType) {
        //     case WeaponType::SHORTBOW:
        //     case WeaponType::LONGBOW:
        //     case WeaponType::CROSSBOW:
        //     case WeaponType::THROWN:
        //         m_CombatType = CombatType::RANGED;
        //         break;
        //     case WeaponType::STAFF:
        //         // Staff can be melee or magic depending on spell selected
        //         // For now, default to melee
        //         m_CombatType = CombatType::MELEE;
        //         break;
        //     default:
        //         m_CombatType = CombatType::MELEE;
        //         break;
        // }
    }

}
