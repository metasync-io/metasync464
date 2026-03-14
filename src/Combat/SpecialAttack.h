#pragma once

#include <cstdint>
#include <string>

namespace Skeleton {

    /**
     * Special attack effect types
     */
    enum class SpecialEffect : uint8_t {
        NONE = 0,
        INCREASED_DAMAGE = 1,      // Simple damage multiplier
        INCREASED_ACCURACY = 2,     // Accuracy multiplier
        MULTI_HIT = 3,              // Multiple hits (Dragon Claws)
        GUARANTEED_HIT = 4,         // Always hits
        HEAL = 5,                   // Heals on hit (SGS, Guthans)
        DRAIN_STATS = 6,            // Drains opponent stats (AGS, SWH)
        FREEZE = 7,                 // Freezes opponent (ZGS)
        SPECIAL_DAMAGE = 8,         // Special damage calculation (DDS double hit, etc.)
        PROTECTION = 9,             // Damage reduction (Elysian, DFS)
        POISON = 10,                // Inflicts poison
        STUN = 11                   // Stuns opponent
    };

    /**
     * Represents a weapon special attack
     */
    struct SpecialAttack {
        int32_t m_WeaponId;              // Weapon item ID
        const char* m_Name;              // Special attack name (e.g., "Slice and Dice")
        int32_t m_EnergyCost;            // Energy cost (0-100, typically 25, 50, or 100)
        double m_DamageMultiplier;       // Damage multiplier (1.0 = normal, 1.15 = 15% boost)
        double m_AccuracyMultiplier;     // Accuracy multiplier
        int32_t m_HitCount;              // Number of hits (1-4)
        SpecialEffect m_Effect;          // Special effect type
        int32_t m_SpecialAnimation;      // Special attack animation ID
        int32_t m_SpecialGraphic;        // Special attack graphic ID

        constexpr SpecialAttack()
            : m_WeaponId(-1), m_Name(""), m_EnergyCost(0),
              m_DamageMultiplier(1.0), m_AccuracyMultiplier(1.0),
              m_HitCount(1), m_Effect(SpecialEffect::NONE),
              m_SpecialAnimation(-1), m_SpecialGraphic(-1) {}

        constexpr SpecialAttack(int32_t weaponId, const char* name, int32_t cost,
                               double dmgMult, double accMult, int32_t hits = 1,
                               SpecialEffect effect = SpecialEffect::NONE,
                               int32_t anim = -1, int32_t gfx = -1)
            : m_WeaponId(weaponId), m_Name(name), m_EnergyCost(cost),
              m_DamageMultiplier(dmgMult), m_AccuracyMultiplier(accMult),
              m_HitCount(hits), m_Effect(effect),
              m_SpecialAnimation(anim), m_SpecialGraphic(gfx) {}

        /**
         * Checks if this special attack is valid
         */
        constexpr bool IsValid() const {
            return m_WeaponId != -1 && m_EnergyCost > 0;
        }
    };

}
