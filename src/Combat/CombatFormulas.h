#pragma once

#include <cstdint>
#include "CombatState.h"
#include "../Skills/Skill.h"

namespace Skeleton {

    class Player;
    class Mob;

    // Pure functions for combat calculations
    // All functions are const and have no side effects
    namespace CombatFormulas {

        // ========== Melee Formulas ==========

        /**
         * Calculates the effective strength level for melee
         * Formula: (strLevel * prayerBonus * otherBonus) + styleBonus
         */
        double GetEffectiveStrength(const Player& player, bool special = false);

        /**
         * Calculates the maximum melee hit
         * Formula: (5 + ((effectiveStr + 8) * (strBonus + 64)) / 64) * specialMult
         */
        int32_t CalculateMeleeMaxHit(const Player& player, bool special = false);

        /**
         * Gets the melee strength bonus from equipment
         */
        int32_t GetMeleeStrengthBonus(const Player& player);

        /**
         * Gets the melee attack bonus (highest of stab/slash/crush)
         */
        int32_t GetMeleeAttackBonus(const Player& player);

        // ========== Ranged Formulas ==========

        /**
         * Calculates the effective ranged level
         */
        double GetEffectiveRanged(const Player& player, bool special = false);

        /**
         * Calculates the maximum ranged hit
         */
        int32_t CalculateRangedMaxHit(const Player& player, bool special = false);

        /**
         * Gets the ranged strength bonus from equipment
         */
        int32_t GetRangedStrengthBonus(const Player& player);

        /**
         * Gets the ranged attack bonus from equipment
         */
        int32_t GetRangedAttackBonus(const Player& player);

        // ========== Magic Formulas ==========

        /**
         * Calculates the effective magic level
         */
        double GetEffectiveMagic(const Player& player);

        /**
         * Calculates the maximum magic hit for a spell
         */
        int32_t CalculateMagicMaxHit(const Player& player, int32_t spellId);

        /**
         * Gets the magic attack bonus from equipment
         */
        int32_t GetMagicAttackBonus(const Player& player);

        // ========== Accuracy System ==========

        /**
         * Rolls accuracy check between attacker and victim
         * Returns true if the attack hits, false if it misses
         */
        bool RollAccuracy(const Player& attacker, const Player& victim,
                         CombatType type, bool special = false);

        /**
         * Calculates the attack roll for the attacker
         * Formula: (floor(equipBonus + attackLvl) + 8) * prayerMod * styleMod
         */
        double GetAttackRoll(const Player& attacker, CombatType type, bool special = false);

        /**
         * Calculates the defence roll for the victim
         * Formula: (floor(equipBonus + defenceLvl) + 8) * prayerMod
         */
        double GetDefenceRoll(const Player& victim, CombatType type);

        /**
         * Calculates hit chance from attack and defence rolls
         * Formula: if A < D: (A-1)/(2D), else: 1 - (D+1)/(2A)
         * Clamped between 0.01 and 0.99
         */
        double CalculateHitChance(double attackRoll, double defenceRoll);

        // ========== Utility Functions ==========

        /**
         * Gets the prayer multiplier for attack
         */
        double GetPrayerAttackMultiplier(const Player& player, CombatType type);

        /**
         * Gets the prayer multiplier for strength/ranged str/magic dmg
         */
        double GetPrayerStrengthMultiplier(const Player& player, CombatType type);

        /**
         * Gets the prayer multiplier for defence
         */
        double GetPrayerDefenceMultiplier(const Player& player, CombatType type);

        /**
         * Gets the style bonus for attack
         * Accurate = +3, Controlled = +1, others = 0
         */
        int32_t GetAttackStyleBonus(AttackStyle style);

        /**
         * Gets the style bonus for strength
         * Aggressive = +3, Controlled = +1, others = 0
         */
        int32_t GetStrengthStyleBonus(AttackStyle style);

        /**
         * Gets the style bonus for defence
         * Defensive = +3, Controlled = +1, Longrange = +3, others = 0
         */
        int32_t GetDefenceStyleBonus(AttackStyle style);

        /**
         * Gets the highest attack bonus index (0=stab, 1=slash, 2=crush)
         */
        int32_t GetHighestAttackBonusIndex(const Player& player);

    }

}
