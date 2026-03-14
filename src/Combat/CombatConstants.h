#pragma once

#include <cstdint>
#include <unordered_map>

namespace Skeleton {

    // Combat constants and weapon speed definitions
    namespace CombatConstants {

        // Default attack speeds (in ticks, 600ms per tick)
        constexpr int32_t DEFAULT_ATTACK_SPEED = 4;
        constexpr int32_t MELEE_DISTANCE = 1;
        constexpr int32_t HALBERD_DISTANCE = 2;
        constexpr int32_t RANGED_DISTANCE = 10;
        constexpr int32_t MAGIC_DISTANCE = 10;

        // Special energy
        constexpr int32_t MAX_SPECIAL_ENERGY = 100;
        constexpr int32_t SPECIAL_ENERGY_REGEN_RATE = 10; // Per minute (100 ticks)

        // Weapon speed map (item ID -> attack speed in ticks)
        // Speed 4 = 2.4s, Speed 5 = 3.0s, Speed 6 = 3.6s, etc.
        inline const std::unordered_map<int32_t, int32_t> WEAPON_SPEEDS = {
            // Scimitars (speed 4)
            {1333, 4}, // Rune scimitar
            {4587, 4}, // Dragon scimitar

            // Daggers (speed 4)
            {1215, 4}, // Dragon dagger
            {1231, 4}, // Dragon dagger (p)
            {5680, 4}, // Dragon dagger (p+)
            {5698, 4}, // Dragon dagger (p++)

            // Whips (speed 4)
            {4151, 4}, // Abyssal whip

            // Longswords (speed 5)
            {1289, 5}, // Rune longsword
            {1305, 5}, // Dragon longsword

            // 2h swords (speed 6)
            {1319, 6}, // Rune 2h sword

            // Godswords (speed 6)
            {11694, 6}, // Armadyl godsword
            {11696, 6}, // Bandos godsword
            {11698, 6}, // Saradomin godsword
            {11700, 6}, // Zamorak godsword

            // Granite maul (speed 7)
            {4153, 7}, // Granite maul

            // Halberds (speed 7)
            {3204, 7}, // Dragon halberd

            // Bows (speed 5-6)
            {839, 5},  // Longbow
            {841, 6},  // Oak longbow
            {847, 6},  // Magic longbow
            {861, 4},  // Shortbow
            {11235, 9}, // Dark bow
        };

        // Weapon reach (item ID -> attack distance)
        inline const std::unordered_map<int32_t, int32_t> WEAPON_REACH = {
            // Halberds have 2 tile reach
            {3204, 2}, // Dragon halberd
        };

        // Get weapon attack speed (returns default if not found)
        inline int32_t GetWeaponSpeed(int32_t weaponId) {
            auto it = WEAPON_SPEEDS.find(weaponId);
            return it != WEAPON_SPEEDS.end() ? it->second : DEFAULT_ATTACK_SPEED;
        }

        // Get weapon reach (returns 1 if not found)
        inline int32_t GetWeaponReach(int32_t weaponId) {
            auto it = WEAPON_REACH.find(weaponId);
            return it != WEAPON_REACH.end() ? it->second : MELEE_DISTANCE;
        }

    }

}
