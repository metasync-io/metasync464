#pragma once

#include <cstdint>
#include <array>

namespace Skeleton {

    /**
     * Represents a rune requirement for a spell
     */
    struct RuneRequirement {
        int32_t m_ItemId;    // Rune item ID
        int32_t m_Amount;    // Amount required

        constexpr RuneRequirement(int32_t itemId = -1, int32_t amount = 0)
            : m_ItemId(itemId), m_Amount(amount) {}
    };

    /**
     * Represents a magic spell in the game
     */
    struct Spell {
        int32_t m_SpellId;              // Button ID / spell interface ID
        int32_t m_RequiredLevel;        // Required magic level
        int32_t m_BaseMaxHit;           // Base damage without equipment bonuses
        int32_t m_Experience;           // XP gained on cast
        int32_t m_ProjectileId;         // Projectile graphics ID
        int32_t m_StartGraphicId;       // Graphic shown on caster
        int32_t m_EndGraphicId;         // Graphic shown on target
        int32_t m_CastAnimation;        // Animation when casting
        int32_t m_CastSpeed;            // Speed of the spell (affects attack timer)
        std::array<RuneRequirement, 5> m_Runes; // Up to 5 different rune types

        constexpr Spell()
            : m_SpellId(-1), m_RequiredLevel(1), m_BaseMaxHit(0),
              m_Experience(0), m_ProjectileId(-1), m_StartGraphicId(-1),
              m_EndGraphicId(-1), m_CastAnimation(0), m_CastSpeed(5), m_Runes{} {}

        constexpr Spell(int32_t spellId, int32_t level, int32_t maxHit, int32_t xp,
                       int32_t projId, int32_t startGfx, int32_t endGfx, int32_t castAnim = 0,
                       int32_t castSpeed = 5)
            : m_SpellId(spellId), m_RequiredLevel(level), m_BaseMaxHit(maxHit),
              m_Experience(xp), m_ProjectileId(projId), m_StartGraphicId(startGfx),
              m_EndGraphicId(endGfx), m_CastAnimation(castAnim), m_CastSpeed(castSpeed),
              m_Runes{} {}

        /**
         * Adds a rune requirement to the spell
         * @param index Index in the rune array (0-4)
         * @param itemId Rune item ID
         * @param amount Amount required
         */
        constexpr Spell& WithRune(size_t index, int32_t itemId, int32_t amount) {
            if (index < m_Runes.size()) {
                m_Runes[index] = RuneRequirement(itemId, amount);
            }
            return *this;
        }

        /**
         * Checks if the spell has a valid configuration
         */
        constexpr bool IsValid() const {
            return m_SpellId != -1 && m_RequiredLevel > 0;
        }
    };

}
