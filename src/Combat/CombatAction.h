#pragma once

#include <cstdint>
#include "Hit.h"
#include "CombatState.h"

namespace Skeleton {

    class Player;
    class Mob;

    /**
     * Abstract base class for combat actions
     * Defines the interface for different combat styles (Melee, Ranged, Magic)
     */
    class CombatAction {
    public:
        virtual ~CombatAction() = default;

        /**
         * Checks if the attacker can attack the victim
         * This includes checks like wilderness level, combat requirements, etc.
         */
        virtual bool CanAttack(Player& attacker, Player& victim) const;

        /**
         * Checks if the attacker is within attack radius of the victim
         */
        virtual bool IsWithinRadius(const Player& attacker, const Player& victim) const = 0;

        /**
         * Begins the combat action (plays animation, sends projectile, etc.)
         */
        virtual void Begin(Player& attacker, Player& victim, bool special = false) = 0;

        /**
         * Calculates the damage for this attack
         */
        virtual Hit CalculateDamage(Player& attacker, Player& victim, bool special = false) const = 0;

        /**
         * Executes a special attack if the weapon supports it
         * Returns true if special attack was executed
         */
        virtual bool ExecuteSpecialAttack(Player& attacker, Player& victim) = 0;

    protected:
        /**
         * Applies a hit to the victim after a delay
         */
        void ApplyHit(Player& victim, const Hit& hit, int32_t delayTicks);

        /**
         * Gets the attack speed for the player's current weapon
         */
        int32_t GetAttackSpeed(const Player& player) const;

        /**
         * Gets the attack animation for the player's current weapon
         */
        int32_t GetAttackAnimation(const Player& player) const;

        /**
         * Gets the block animation for the player
         */
        int32_t GetBlockAnimation(const Player& player) const;

        /**
         * Gets the weapon item ID from player's equipment
         */
        int32_t GetWeaponId(const Player& player) const;
    };

}
