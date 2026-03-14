#include "MeleeAction.h"
#include "../Player.h"
#include "../Position.h"
#include "CombatHandler.h"
#include "CombatFormulas.h"
#include "CombatConstants.h"
#include "SpecialAttackData.h"
#include <random>
#include <cmath>

namespace Skeleton {

    bool MeleeAction::IsWithinRadius(const Player& attacker, const Player& victim) const {
        int32_t distance = GetAttackDistance(attacker);
        int32_t dx = static_cast<int32_t>(std::abs(static_cast<int>(attacker.GetPosition().GetX() - victim.GetPosition().GetX())));
        int32_t dy = static_cast<int32_t>(std::abs(static_cast<int>(attacker.GetPosition().GetY() - victim.GetPosition().GetY())));

        // Check if within attack distance
        return dx <= distance && dy <= distance;
    }

    void MeleeAction::Begin(Player& attacker, Player& victim, bool special) {
        // Calculate damage
        Hit hit = CalculateDamage(attacker, victim, special);

        // Play attack animation
        int32_t attackAnim = GetAttackAnimation(attacker);
        attacker.PlayAnimation(attackAnim);

        // TODO: Set facing direction toward victim
        // attacker.SetInteractingEntity(victim);

        // Apply the hit with a delay (melee hits after 1 tick)
        ApplyHit(victim, hit, 1);

        // Set attack timer
        int32_t attackSpeed = GetAttackSpeed(attacker);
        // TODO: Get combat handler from attacker
        // attacker.GetCombatHandler().SetAttackTimer(attackSpeed);

        // Refresh combat timestamps
        // TODO: attacker.GetCombatHandler().RefreshLastAttack();
        // TODO: victim.GetCombatHandler().RefreshLastHit();
    }

    Hit MeleeAction::CalculateDamage(Player& attacker, Player& victim, bool special) const {
        // Roll accuracy
        bool accurate = CombatFormulas::RollAccuracy(attacker, victim, CombatType::MELEE, special);

        if (!accurate) {
            // Miss - return block hit with 0 damage
            return Hit(std::weak_ptr<Player>(), 0, HitType::BLOCK);
        }

        // Hit - roll damage
        int32_t damage = RollDamage(attacker, victim, special);

        // TODO: Clamp damage to victim's current HP
        // damage = std::min(damage, victim.GetCurrentHitpoints());

        return Hit(std::weak_ptr<Player>(), damage, HitType::NORMAL);
    }

    bool MeleeAction::ExecuteSpecialAttack(Player& attacker, Player& victim) {
        int32_t weaponId = GetWeaponId(attacker);

        // Get special attack definition
        const SpecialAttack* special = GetSpecialAttack(weaponId);
        if (!special || !special->IsValid()) {
            return false; // No special attack for this weapon
        }

        // Check if player has enough special energy
        if (!attacker.GetCombatHandler().CanUseSpecial(special->m_EnergyCost)) {
            return false;
        }

        // Play special attack animation
        int32_t specialAnim = special->m_SpecialAnimation > 0 ?
                              special->m_SpecialAnimation : GetAttackAnimation(attacker);
        attacker.PlayAnimation(specialAnim);

        // Display special attack graphic
        if (special->m_SpecialGraphic > 0) {
            // TODO: Play graphic when graphics system is available
            // attacker.PlayGraphic(special->m_SpecialGraphic, 0, 0);
        }

        // Execute the special attack based on hit count
        for (int32_t i = 0; i < special->m_HitCount; i++) {
            Hit hit = CalculateDamage(attacker, victim, true);
            ApplyHit(victim, hit, 1 + i); // Stagger multi-hits
        }

        // Consume special energy
        attacker.GetCombatHandler().ConsumeSpecialEnergy(special->m_EnergyCost);

        // Apply special effects (healing, stat drain, etc.)
        ApplySpecialEffects(attacker, victim, *special);

        return true;
    }

    int32_t MeleeAction::GetAttackDistance(const Player& player) const {
        int32_t weaponId = GetWeaponId(player);
        return CombatConstants::GetWeaponReach(weaponId);
    }

    int32_t MeleeAction::RollDamage(Player& attacker, Player& victim, bool special) const {
        int32_t maxHit = CombatFormulas::CalculateMeleeMaxHit(attacker, special);

        // Roll random damage between 0 and maxHit
        static std::random_device rd;
        static std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, maxHit);

        return dis(gen);
    }

    int32_t MeleeAction::GetSpecialEnergyCost(int32_t weaponId) const {
        const SpecialAttack* special = GetSpecialAttack(weaponId);
        return special ? special->m_EnergyCost : 0;
    }

    int32_t MeleeAction::GetSpecialHitCount(int32_t weaponId) const {
        const SpecialAttack* special = GetSpecialAttack(weaponId);
        return special ? special->m_HitCount : 1;
    }

    void MeleeAction::ApplySpecialEffects(Player& attacker, Player& victim, const SpecialAttack& special) const {
        switch (special.m_Effect) {
            case SpecialEffect::HEAL:
                // Saradomin Godsword - heal for 50% of damage dealt
                // TODO: Implement healing when HP system is available
                // int32_t healAmount = lastDamage / 2;
                // attacker.Heal(healAmount);
                break;

            case SpecialEffect::DRAIN_STATS:
                // Bandos Godsword, Statius's Warhammer - drain stats
                // TODO: Implement stat draining when stats system is available
                break;

            case SpecialEffect::FREEZE:
                // Zamorak Godsword - freeze opponent
                // TODO: Implement freeze when movement system supports it
                break;

            case SpecialEffect::PROTECTION:
                // Dragon Scimitar - damage reduction
                // TODO: Implement damage reduction buff
                break;

            default:
                // No additional effects
                break;
        }
    }

}
