#pragma once

#include "SpecialAttack.h"
#include <array>

namespace Skeleton {

    /**
     * All weapon special attack definitions
     */
    namespace SpecialAttacks {

        // ===== MELEE WEAPONS =====

        // Dragon Dagger (2 quick hits with increased accuracy)
        constexpr SpecialAttack DRAGON_DAGGER = SpecialAttack(
            1215, "Puncture", 25, 1.15, 1.25, 2, SpecialEffect::MULTI_HIT, 1062, 252
        );
        constexpr SpecialAttack DRAGON_DAGGER_P = SpecialAttack(
            1231, "Puncture", 25, 1.15, 1.25, 2, SpecialEffect::MULTI_HIT, 1062, 252
        );
        constexpr SpecialAttack DRAGON_DAGGER_PP = SpecialAttack(
            5680, "Puncture", 25, 1.15, 1.25, 2, SpecialEffect::MULTI_HIT, 1062, 252
        );

        // Dragon Claws (4 hits with special damage calculation)
        constexpr SpecialAttack DRAGON_CLAWS = SpecialAttack(
            13652, "Slice and Dice", 50, 1.0, 1.0, 4, SpecialEffect::SPECIAL_DAMAGE, 10961, 1950
        );

        // Armadyl Godsword (Increased damage and accuracy)
        constexpr SpecialAttack ARMADYL_GODSWORD = SpecialAttack(
            11694, "The Judgment", 50, 1.375, 2.0, 1, SpecialEffect::INCREASED_DAMAGE, 11989, 2113
        );

        // Bandos Godsword (Drain opponent stats)
        constexpr SpecialAttack BANDOS_GODSWORD = SpecialAttack(
            11696, "Warstrike", 50, 1.21, 1.0, 1, SpecialEffect::DRAIN_STATS, 11991, 2114
        );

        // Saradomin Godsword (Heal and restore prayer)
        constexpr SpecialAttack SARADOMIN_GODSWORD = SpecialAttack(
            11698, "Healing Blade", 50, 1.1, 1.0, 1, SpecialEffect::HEAL, 11993, 2109
        );

        // Zamorak Godsword (Freeze opponent)
        constexpr SpecialAttack ZAMORAK_GODSWORD = SpecialAttack(
            11700, "Ice Cleave", 50, 1.1, 1.0, 1, SpecialEffect::FREEZE, 11995, 2111
        );

        // Abyssal Whip (Increased accuracy, drains run energy)
        constexpr SpecialAttack ABYSSAL_WHIP = SpecialAttack(
            4151, "Energy Drain", 50, 1.0, 1.25, 1, SpecialEffect::DRAIN_STATS, 11971, 2108
        );

        // Granite Maul (Instant attack, no delay)
        constexpr SpecialAttack GRANITE_MAUL = SpecialAttack(
            4153, "Quick Smash", 50, 1.0, 1.0, 1, SpecialEffect::NONE, 1667, 340
        );

        // Dragon Longsword
        constexpr SpecialAttack DRAGON_LONGSWORD = SpecialAttack(
            1305, "Cleave", 25, 1.25, 1.0, 1, SpecialEffect::INCREASED_DAMAGE, 12033, 248
        );

        // Dragon Scimitar
        constexpr SpecialAttack DRAGON_SCIMITAR = SpecialAttack(
            4587, "Sever", 55, 1.0, 1.25, 1, SpecialEffect::PROTECTION, 12031, 347
        );

        // Dragon Mace
        constexpr SpecialAttack DRAGON_MACE = SpecialAttack(
            1434, "Shatter", 25, 1.5, 1.0, 1, SpecialEffect::INCREASED_DAMAGE, 1060, 251
        );

        // Dragon Battleaxe (Boosts strength, drains other stats)
        constexpr SpecialAttack DRAGON_BATTLEAXE = SpecialAttack(
            1377, "Rampage", 100, 1.0, 1.0, 1, SpecialEffect::DRAIN_STATS, 1056, 246
        );

        // Dragon 2H Sword
        constexpr SpecialAttack DRAGON_2H = SpecialAttack(
            7158, "Powerstab", 60, 1.0, 1.0, 1, SpecialEffect::NONE, 3157, 559
        );

        // Vesta's Longsword
        constexpr SpecialAttack VESTAS_LONGSWORD = SpecialAttack(
            13899, "Feint", 25, 1.2, 1.0, 1, SpecialEffect::INCREASED_DAMAGE, 10502, 1835
        );

        // Statius's Warhammer (Drains defence)
        constexpr SpecialAttack STATIUS_WARHAMMER = SpecialAttack(
            13902, "Smash", 35, 1.25, 1.0, 1, SpecialEffect::DRAIN_STATS, 10505, 1840
        );

        // ===== RANGED WEAPONS =====

        // Dark Bow (2 hits, guaranteed minimum damage)
        constexpr SpecialAttack DARK_BOW = SpecialAttack(
            11235, "Descent of Darkness", 55, 1.3, 1.0, 2, SpecialEffect::MULTI_HIT, 426, 1100
        );

        // Magic Shortbow (2 rapid hits)
        constexpr SpecialAttack MAGIC_SHORTBOW = SpecialAttack(
            861, "Snap-Shot", 55, 1.0, 1.0, 2, SpecialEffect::MULTI_HIT, 1074, 250
        );

        // Rune Throwing Axe
        constexpr SpecialAttack RUNE_THROWING_AXE = SpecialAttack(
            809, "Chain Hit", 50, 1.1, 1.0, 1, SpecialEffect::NONE, 929, 256
        );

        // Dragon Knife
        constexpr SpecialAttack DRAGON_KNIFE = SpecialAttack(
            22812, "Sudden Death", 25, 1.25, 1.0, 2, SpecialEffect::MULTI_HIT, 8194, 1621
        );

        // Dragon Javelin
        constexpr SpecialAttack DRAGON_JAVELIN = SpecialAttack(
            19484, "Piercing Shot", 25, 1.35, 1.0, 1, SpecialEffect::INCREASED_DAMAGE, 806, 1301
        );

        // Ballista (Heavy Ballista)
        constexpr SpecialAttack HEAVY_BALLISTA = SpecialAttack(
            19481, "Unload", 65, 1.25, 1.25, 1, SpecialEffect::INCREASED_DAMAGE, 7555, 1302
        );

        // ===== COMBINED WEAPONS =====

        // Toxic Blowpipe
        constexpr SpecialAttack TOXIC_BLOWPIPE = SpecialAttack(
            12926, "Toxic Siphon", 50, 1.5, 1.0, 1, SpecialEffect::HEAL, 5061, 1123
        );

        // Array of all special attacks for lookup
        constexpr std::array<SpecialAttack, 25> ALL_SPECIALS = {
            DRAGON_DAGGER, DRAGON_DAGGER_P, DRAGON_DAGGER_PP,
            DRAGON_CLAWS,
            ARMADYL_GODSWORD, BANDOS_GODSWORD, SARADOMIN_GODSWORD, ZAMORAK_GODSWORD,
            ABYSSAL_WHIP, GRANITE_MAUL,
            DRAGON_LONGSWORD, DRAGON_SCIMITAR, DRAGON_MACE, DRAGON_BATTLEAXE, DRAGON_2H,
            VESTAS_LONGSWORD, STATIUS_WARHAMMER,
            DARK_BOW, MAGIC_SHORTBOW,
            RUNE_THROWING_AXE, DRAGON_KNIFE, DRAGON_JAVELIN, HEAVY_BALLISTA,
            TOXIC_BLOWPIPE
        };
    }

    /**
     * Gets the special attack definition for a weapon
     * @param weaponId The weapon item ID
     * @return Pointer to SpecialAttack if found, nullptr otherwise
     */
    inline const SpecialAttack* GetSpecialAttack(int32_t weaponId) {
        for (const auto& special : SpecialAttacks::ALL_SPECIALS) {
            if (special.m_WeaponId == weaponId) {
                return &special;
            }
        }
        return nullptr;
    }

    /**
     * Checks if a weapon has a special attack
     */
    inline bool HasSpecialAttack(int32_t weaponId) {
        return GetSpecialAttack(weaponId) != nullptr;
    }

}
