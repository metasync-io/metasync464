#pragma once

#include "CombatAction.h"
#include "SpecialAttack.h"
#include <memory>

namespace Skeleton {

    /**
     * Melee combat action implementation
     * Handles melee attacks with weapons or unarmed combat
     */
    class MeleeAction : public CombatAction {
    public:
        MeleeAction() = default;
        virtual ~MeleeAction() = default;

        bool IsWithinRadius(const Player& attacker, const Player& victim) const override;

        void Begin(Player& attacker, Player& victim, bool special = false) override;

        Hit CalculateDamage(Player& attacker, Player& victim, bool special = false) const override;

        bool ExecuteSpecialAttack(Player& attacker, Player& victim) override;

    private:
        /**
         * Gets the attack distance for the weapon (halberds have 2 tile reach)
         */
        int32_t GetAttackDistance(const Player& player) const;

        /**
         * Calculates damage for a melee hit
         */
        int32_t RollDamage(Player& attacker, Player& victim, bool special) const;

        /**
         * Gets the special attack energy cost for the weapon
         */
        int32_t GetSpecialEnergyCost(int32_t weaponId) const;

        /**
         * Gets the number of hits for a special attack (e.g., Dragon Claws = 4)
         */
        int32_t GetSpecialHitCount(int32_t weaponId) const;

        /**
         * Applies special effects (healing, stat drain, etc.)
         */
        void ApplySpecialEffects(Player& attacker, Player& victim, const SpecialAttack& special) const;
    };

}
