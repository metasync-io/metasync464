#pragma once

#include "CombatAction.h"
#include "Projectile.h"
#include <memory>

namespace Skeleton {

    /**
     * Ranged combat action implementation
     * Handles ranged attacks with bows, crossbows, and thrown weapons
     */
    class RangedAction : public CombatAction {
    public:
        RangedAction() = default;
        virtual ~RangedAction() = default;

        bool IsWithinRadius(const Player& attacker, const Player& victim) const override;

        void Begin(Player& attacker, Player& victim, bool special = false) override;

        Hit CalculateDamage(Player& attacker, Player& victim, bool special = false) const override;

        bool ExecuteSpecialAttack(Player& attacker, Player& victim) override;

    private:
        /**
         * Gets the maximum attack distance for ranged weapons
         * Longbows have longer range than shortbows, crossbows, etc.
         */
        int32_t GetAttackDistance(const Player& player) const;

        /**
         * Calculates damage for a ranged hit
         */
        int32_t RollDamage(Player& attacker, Player& victim, bool special) const;

        /**
         * Checks if the player has the required ammunition
         */
        bool HasRequiredAmmo(const Player& player, int32_t weaponId, int32_t& ammoId) const;

        /**
         * Checks if the ammunition is compatible with the weapon
         */
        bool IsAmmoCompatible(int32_t weaponId, int32_t ammoId) const;

        /**
         * Consumes ammunition after firing
         * @param player The player using ammunition
         * @param ammoId The ammunition item ID
         * @param amount Number of ammo to consume
         */
        void ConsumeAmmo(Player& player, int32_t ammoId, int32_t amount) const;

        /**
         * Gets the projectile ID for the ammunition/weapon combination
         */
        int32_t GetProjectileId(int32_t weaponId, int32_t ammoId) const;

        /**
         * Gets the special attack energy cost for the weapon
         */
        int32_t GetSpecialEnergyCost(int32_t weaponId) const;

        /**
         * Gets the number of hits for a special attack (e.g., Dark Bow = 2)
         */
        int32_t GetSpecialHitCount(int32_t weaponId) const;

        /**
         * Gets the drawback graphic ID (the animation when pulling back bow)
         */
        int32_t GetDrawbackGraphic(int32_t weaponId, int32_t ammoId) const;

        /**
         * Displays the ranged projectile from attacker to victim
         */
        void DisplayProjectile(Player& attacker, Player& victim, int32_t weaponId, int32_t ammoId) const;

        /**
         * Gets list of compatible ammo IDs for a weapon
         */
        std::vector<int32_t> GetCompatibleAmmoIds(int32_t weaponId) const;

        /**
         * Checks if weapon has infinite ammo (crystal bow, etc.)
         */
        bool IsInfiniteAmmoWeapon(int32_t weaponId) const;

        /**
         * Gets default projectile for infinite ammo weapons
         */
        int32_t GetDefaultProjectileForWeapon(int32_t weaponId) const;
    };

}
