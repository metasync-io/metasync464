#pragma once

#include <cstdint>

namespace Skeleton {

    /**
     * Rune item IDs for spell requirements
     */
    namespace RuneIds {
        // Elemental runes
        constexpr int32_t AIR_RUNE = 556;
        constexpr int32_t WATER_RUNE = 555;
        constexpr int32_t EARTH_RUNE = 557;
        constexpr int32_t FIRE_RUNE = 554;

        // Catalytic runes
        constexpr int32_t MIND_RUNE = 558;
        constexpr int32_t CHAOS_RUNE = 562;
        constexpr int32_t DEATH_RUNE = 560;
        constexpr int32_t BLOOD_RUNE = 565;
        constexpr int32_t SOUL_RUNE = 566;
        constexpr int32_t ASTRAL_RUNE = 9075;
        constexpr int32_t NATURE_RUNE = 561;
        constexpr int32_t LAW_RUNE = 563;
        constexpr int32_t COSMIC_RUNE = 564;
        constexpr int32_t BODY_RUNE = 559;

        // Combination runes (save inventory space)
        constexpr int32_t MIST_RUNE = 4695;     // Air + Water
        constexpr int32_t DUST_RUNE = 4696;     // Air + Earth
        constexpr int32_t MUD_RUNE = 4698;      // Water + Earth
        constexpr int32_t SMOKE_RUNE = 4697;    // Air + Fire
        constexpr int32_t STEAM_RUNE = 4694;    // Water + Fire
        constexpr int32_t LAVA_RUNE = 4699;     // Earth + Fire

        // Wrath rune (OSRS)
        constexpr int32_t WRATH_RUNE = 21880;
    }

    /**
     * Checks if an item is a rune
     */
    inline bool IsRune(int32_t itemId) {
        return (itemId >= 554 && itemId <= 566) ||   // Basic runes
               (itemId >= 4694 && itemId <= 4699) ||  // Combination runes
               itemId == 9075 ||                      // Astral
               itemId == 21880;                       // Wrath
    }

    /**
     * Checks if an item is an elemental rune or combination rune
     * This is used for staff substitution (e.g., staff of fire provides infinite fire runes)
     */
    inline bool IsElementalRune(int32_t itemId) {
        return itemId == RuneIds::AIR_RUNE ||
               itemId == RuneIds::WATER_RUNE ||
               itemId == RuneIds::EARTH_RUNE ||
               itemId == RuneIds::FIRE_RUNE ||
               (itemId >= 4694 && itemId <= 4699); // Combination runes
    }

}
