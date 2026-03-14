#pragma once

#include "Spell.h"
#include "RuneDefinition.h"
#include <array>

namespace Skeleton {

    /**
     * Modern spellbook spell definitions
     */
    namespace ModernSpells {
        // Strike spells (level 1-13)
        constexpr Spell WIND_STRIKE = Spell(1152, 1, 2, 6, 91, 90, 92, 711, 5)
            .WithRune(0, RuneIds::AIR_RUNE, 1)
            .WithRune(1, RuneIds::MIND_RUNE, 1);

        constexpr Spell WATER_STRIKE = Spell(1154, 5, 4, 8, 94, 93, 95, 711, 5)
            .WithRune(0, RuneIds::WATER_RUNE, 1)
            .WithRune(1, RuneIds::AIR_RUNE, 1)
            .WithRune(2, RuneIds::MIND_RUNE, 1);

        constexpr Spell EARTH_STRIKE = Spell(1156, 9, 6, 10, 97, 96, 98, 711, 5)
            .WithRune(0, RuneIds::EARTH_RUNE, 1)
            .WithRune(1, RuneIds::AIR_RUNE, 1)
            .WithRune(2, RuneIds::MIND_RUNE, 1);

        constexpr Spell FIRE_STRIKE = Spell(1158, 13, 8, 12, 100, 99, 101, 711, 5)
            .WithRune(0, RuneIds::FIRE_RUNE, 2)
            .WithRune(1, RuneIds::AIR_RUNE, 1)
            .WithRune(2, RuneIds::MIND_RUNE, 1);

        // Bolt spells (level 17-35)
        constexpr Spell WIND_BOLT = Spell(1160, 17, 9, 14, 118, 117, 119, 711, 5)
            .WithRune(0, RuneIds::AIR_RUNE, 2)
            .WithRune(1, RuneIds::CHAOS_RUNE, 1);

        constexpr Spell WATER_BOLT = Spell(1163, 23, 10, 17, 121, 120, 122, 711, 5)
            .WithRune(0, RuneIds::WATER_RUNE, 2)
            .WithRune(1, RuneIds::AIR_RUNE, 2)
            .WithRune(2, RuneIds::CHAOS_RUNE, 1);

        constexpr Spell EARTH_BOLT = Spell(1166, 29, 11, 20, 124, 123, 125, 711, 5)
            .WithRune(0, RuneIds::EARTH_RUNE, 2)
            .WithRune(1, RuneIds::AIR_RUNE, 2)
            .WithRune(2, RuneIds::CHAOS_RUNE, 1);

        constexpr Spell FIRE_BOLT = Spell(1169, 35, 12, 23, 127, 126, 128, 711, 5)
            .WithRune(0, RuneIds::FIRE_RUNE, 3)
            .WithRune(1, RuneIds::AIR_RUNE, 2)
            .WithRune(2, RuneIds::CHAOS_RUNE, 1);

        // Blast spells (level 41-59)
        constexpr Spell WIND_BLAST = Spell(1172, 41, 13, 26, 133, 132, 134, 711, 5)
            .WithRune(0, RuneIds::AIR_RUNE, 3)
            .WithRune(1, RuneIds::DEATH_RUNE, 1);

        constexpr Spell WATER_BLAST = Spell(1175, 47, 14, 29, 136, 135, 137, 711, 5)
            .WithRune(0, RuneIds::WATER_RUNE, 3)
            .WithRune(1, RuneIds::AIR_RUNE, 3)
            .WithRune(2, RuneIds::DEATH_RUNE, 1);

        constexpr Spell EARTH_BLAST = Spell(1177, 53, 15, 32, 139, 138, 140, 711, 5)
            .WithRune(0, RuneIds::EARTH_RUNE, 3)
            .WithRune(1, RuneIds::AIR_RUNE, 3)
            .WithRune(2, RuneIds::DEATH_RUNE, 1);

        constexpr Spell FIRE_BLAST = Spell(1181, 59, 16, 35, 130, 129, 131, 711, 5)
            .WithRune(0, RuneIds::FIRE_RUNE, 4)
            .WithRune(1, RuneIds::AIR_RUNE, 3)
            .WithRune(2, RuneIds::DEATH_RUNE, 1);

        // Wave spells (level 62-75)
        constexpr Spell WIND_WAVE = Spell(1183, 62, 17, 37, 159, 158, 160, 711, 5)
            .WithRune(0, RuneIds::AIR_RUNE, 5)
            .WithRune(1, RuneIds::BLOOD_RUNE, 1);

        constexpr Spell WATER_WAVE = Spell(1185, 65, 18, 38, 162, 161, 163, 711, 5)
            .WithRune(0, RuneIds::WATER_RUNE, 7)
            .WithRune(1, RuneIds::AIR_RUNE, 5)
            .WithRune(2, RuneIds::BLOOD_RUNE, 1);

        constexpr Spell EARTH_WAVE = Spell(1188, 70, 19, 40, 165, 164, 166, 711, 5)
            .WithRune(0, RuneIds::EARTH_RUNE, 7)
            .WithRune(1, RuneIds::AIR_RUNE, 5)
            .WithRune(2, RuneIds::BLOOD_RUNE, 1);

        constexpr Spell FIRE_WAVE = Spell(1189, 75, 20, 43, 156, 155, 157, 711, 5)
            .WithRune(0, RuneIds::FIRE_RUNE, 5)
            .WithRune(1, RuneIds::AIR_RUNE, 5)
            .WithRune(2, RuneIds::BLOOD_RUNE, 1);

        // Surge spells (level 81-95) - OSRS
        constexpr Spell WIND_SURGE = Spell(1190, 81, 21, 45, 168, 167, 169, 711, 5)
            .WithRune(0, RuneIds::AIR_RUNE, 7)
            .WithRune(1, RuneIds::DEATH_RUNE, 1)
            .WithRune(2, RuneIds::BLOOD_RUNE, 1);

        constexpr Spell WATER_SURGE = Spell(1191, 85, 22, 47, 171, 170, 172, 711, 5)
            .WithRune(0, RuneIds::WATER_RUNE, 10)
            .WithRune(1, RuneIds::AIR_RUNE, 7)
            .WithRune(2, RuneIds::DEATH_RUNE, 1)
            .WithRune(3, RuneIds::BLOOD_RUNE, 1);

        constexpr Spell EARTH_SURGE = Spell(1192, 90, 23, 49, 174, 173, 175, 711, 5)
            .WithRune(0, RuneIds::EARTH_RUNE, 10)
            .WithRune(1, RuneIds::AIR_RUNE, 7)
            .WithRune(2, RuneIds::DEATH_RUNE, 1)
            .WithRune(3, RuneIds::BLOOD_RUNE, 1);

        constexpr Spell FIRE_SURGE = Spell(1193, 95, 24, 51, 177, 176, 178, 711, 5)
            .WithRune(0, RuneIds::FIRE_RUNE, 7)
            .WithRune(1, RuneIds::AIR_RUNE, 7)
            .WithRune(2, RuneIds::DEATH_RUNE, 1)
            .WithRune(3, RuneIds::BLOOD_RUNE, 1);

        // Array of all modern combat spells
        constexpr std::array<Spell, 20> ALL_COMBAT_SPELLS = {
            WIND_STRIKE, WATER_STRIKE, EARTH_STRIKE, FIRE_STRIKE,
            WIND_BOLT, WATER_BOLT, EARTH_BOLT, FIRE_BOLT,
            WIND_BLAST, WATER_BLAST, EARTH_BLAST, FIRE_BLAST,
            WIND_WAVE, WATER_WAVE, EARTH_WAVE, FIRE_WAVE,
            WIND_SURGE, WATER_SURGE, EARTH_SURGE, FIRE_SURGE
        };
    }

    /**
     * Ancient Magicks spell definitions
     */
    namespace AncientSpells {
        // Ice spells
        constexpr Spell ICE_RUSH = Spell(12861, 58, 16, 34, 360, 359, 361, 1978, 5)
            .WithRune(0, RuneIds::DEATH_RUNE, 2)
            .WithRune(1, RuneIds::CHAOS_RUNE, 2)
            .WithRune(2, RuneIds::WATER_RUNE, 2);

        constexpr Spell ICE_BURST = Spell(12881, 70, 22, 40, 363, 362, 364, 1978, 5)
            .WithRune(0, RuneIds::DEATH_RUNE, 2)
            .WithRune(1, RuneIds::CHAOS_RUNE, 4)
            .WithRune(2, RuneIds::WATER_RUNE, 4);

        constexpr Spell ICE_BLITZ = Spell(12871, 82, 26, 46, 366, 365, 367, 1978, 5)
            .WithRune(0, RuneIds::DEATH_RUNE, 2)
            .WithRune(1, RuneIds::BLOOD_RUNE, 2)
            .WithRune(2, RuneIds::WATER_RUNE, 3);

        constexpr Spell ICE_BARRAGE = Spell(12891, 94, 30, 52, 369, 368, 370, 1979, 5)
            .WithRune(0, RuneIds::DEATH_RUNE, 4)
            .WithRune(1, RuneIds::BLOOD_RUNE, 2)
            .WithRune(2, RuneIds::WATER_RUNE, 6);

        // Blood spells
        constexpr Spell BLOOD_RUSH = Spell(12901, 56, 15, 33, 372, 371, 373, 1978, 5)
            .WithRune(0, RuneIds::DEATH_RUNE, 2)
            .WithRune(1, RuneIds::CHAOS_RUNE, 2)
            .WithRune(2, RuneIds::BLOOD_RUNE, 1);

        constexpr Spell BLOOD_BURST = Spell(12911, 68, 21, 39, 375, 374, 376, 1978, 5)
            .WithRune(0, RuneIds::DEATH_RUNE, 2)
            .WithRune(1, RuneIds::CHAOS_RUNE, 4)
            .WithRune(2, RuneIds::BLOOD_RUNE, 2);

        constexpr Spell BLOOD_BLITZ = Spell(12929, 80, 25, 45, 378, 377, 379, 1978, 5)
            .WithRune(0, RuneIds::DEATH_RUNE, 2)
            .WithRune(1, RuneIds::BLOOD_RUNE, 4);

        constexpr Spell BLOOD_BARRAGE = Spell(12919, 92, 29, 51, 381, 380, 382, 1979, 5)
            .WithRune(0, RuneIds::DEATH_RUNE, 4)
            .WithRune(1, RuneIds::BLOOD_RUNE, 4)
            .WithRune(2, RuneIds::SOUL_RUNE, 1);
    }

    /**
     * Helper function to get a spell by its ID
     */
    inline const Spell* GetSpellById(int32_t spellId) {
        // Search modern spells
        for (const auto& spell : ModernSpells::ALL_COMBAT_SPELLS) {
            if (spell.m_SpellId == spellId) {
                return &spell;
            }
        }
        // TODO: Search ancient spells
        return nullptr;
    }

}
