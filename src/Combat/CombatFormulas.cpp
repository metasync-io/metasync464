#include "CombatFormulas.h"
#include "../Player.h"
#include "../Equipment/EquipmentHandler.h"
#include "../Equipment/EquipmentBonuses.h"
#include <algorithm>
#include <cmath>
#include <random>

namespace Skeleton {

    namespace CombatFormulas {

        // ========== Melee Formulas ==========

        double GetEffectiveStrength(const Player& player, bool special) {
            double strLevel = static_cast<double>(player.GetSkills().GetEffectiveLevel(Skill::STRENGTH));

            double prayerMod = GetPrayerStrengthMultiplier(player, CombatType::MELEE);
            double otherBonus = 1.0;

            // TODO: Get actual attack style from player
            AttackStyle style = AttackStyle::MELEE_ACCURATE; // Placeholder
            int32_t styleBonus = GetStrengthStyleBonus(style);

            double effectiveStr = (strLevel * prayerMod * otherBonus) + styleBonus;

            // TODO: Add void melee bonus if wearing full void
            // if (wearingFullVoidMelee) {
            //     effectiveStr += ...;
            // }

            return effectiveStr;
        }

        int32_t CalculateMeleeMaxHit(const Player& player, bool special) {
            double effectiveStr = GetEffectiveStrength(player, special);
            int32_t strBonus = GetMeleeStrengthBonus(player);

            // Base damage formula: 5 + ((effectiveStr + 8) * (strBonus + 64)) / 64
            double baseDamage = 5.0 + (((effectiveStr + 8.0) * (strBonus + 64.0)) / 64.0);

            // Apply special attack multiplier if needed
            double specialMult = 1.0;
            if (special) {
                // TODO: Get weapon ID and apply special multipliers
                // For now, default to 1.0
            }

            int32_t maxHit = static_cast<int32_t>(baseDamage * specialMult);
            return std::max(0, maxHit);
        }

        int32_t GetMeleeStrengthBonus(const Player& player) {
            return player.GetEquipmentHandler().GetBonuses().GetBonus(BonusType::STRENGTH_BONUS);
        }

        int32_t GetMeleeAttackBonus(const Player& player) {
            const auto& bonuses = player.GetEquipmentHandler().GetBonuses();
            int32_t stab = bonuses.GetBonus(BonusType::STAB_ATTACK);
            int32_t slash = bonuses.GetBonus(BonusType::SLASH_ATTACK);
            int32_t crush = bonuses.GetBonus(BonusType::CRUSH_ATTACK);
            return std::max({stab, slash, crush});
        }

        // ========== Ranged Formulas ==========

        double GetEffectiveRanged(const Player& player, bool special) {
            double rangedLevel = static_cast<double>(player.GetSkills().GetEffectiveLevel(Skill::RANGED));

            double prayerMod = GetPrayerStrengthMultiplier(player, CombatType::RANGED);
            double otherBonus = 1.0;

            // TODO: Get actual attack style from player
            AttackStyle style = AttackStyle::RANGE_ACCURATE; // Placeholder
            int32_t styleBonus = (style == AttackStyle::RANGE_ACCURATE) ? 3 :
                                 (style == AttackStyle::RANGE_LONGRANGE) ? 1 : 0;

            double effectiveRanged = (rangedLevel * prayerMod * otherBonus) + styleBonus;

            // TODO: Add void ranged bonus if wearing full void

            return effectiveRanged;
        }

        int32_t CalculateRangedMaxHit(const Player& player, bool special) {
            double effectiveRanged = GetEffectiveRanged(player, special);
            int32_t rangedStr = GetRangedStrengthBonus(player);

            // Base damage formula: 5 + ((effectiveRanged + 8) * (rangedStr + 64)) / 64
            double baseDamage = 5.0 + (((effectiveRanged + 8.0) * (rangedStr + 64.0)) / 64.0);

            double specialMult = 1.0;
            if (special) {
                // TODO: Apply special multipliers
            }

            int32_t maxHit = static_cast<int32_t>(baseDamage * specialMult);
            return std::max(0, maxHit);
        }

        int32_t GetRangedStrengthBonus(const Player& player) {
            return player.GetEquipmentHandler().GetBonuses().GetBonus(BonusType::RANGED_STRENGTH);
        }

        int32_t GetRangedAttackBonus(const Player& player) {
            return player.GetEquipmentHandler().GetBonuses().GetBonus(BonusType::RANGED_ATTACK);
        }

        // ========== Magic Formulas ==========

        double GetEffectiveMagic(const Player& player) {
            double magicLevel = static_cast<double>(player.GetSkills().GetEffectiveLevel(Skill::MAGIC));

            double prayerMod = GetPrayerStrengthMultiplier(player, CombatType::MAGIC);
            double otherBonus = 1.0;

            return magicLevel * prayerMod * otherBonus;
        }

        int32_t CalculateMagicMaxHit(const Player& player, int32_t spellId) {
            // TODO: Get spell base damage from spell definitions
            // For now, return a placeholder
            return 20; // Placeholder
        }

        int32_t GetMagicAttackBonus(const Player& player) {
            return player.GetEquipmentHandler().GetBonuses().GetBonus(BonusType::MAGIC_ATTACK);
        }

        // ========== Accuracy System ==========

        bool RollAccuracy(const Player& attacker, const Player& victim,
                         CombatType type, bool special) {
            double attackRoll = GetAttackRoll(attacker, type, special);
            double defenceRoll = GetDefenceRoll(victim, type);
            double hitChance = CalculateHitChance(attackRoll, defenceRoll);

            // Roll random number between 0.0 and 1.0
            static std::random_device rd;
            static std::mt19937 gen(rd());
            std::uniform_real_distribution<> dis(0.0, 1.0);

            return dis(gen) < hitChance;
        }

        double GetAttackRoll(const Player& attacker, CombatType type, bool special) {
            double attackLevel;

            switch (type) {
                case CombatType::MELEE:
                    attackLevel = static_cast<double>(attacker.GetSkills().GetEffectiveLevel(Skill::ATTACK));
                    break;
                case CombatType::RANGED:
                    attackLevel = static_cast<double>(attacker.GetSkills().GetEffectiveLevel(Skill::RANGED));
                    break;
                case CombatType::MAGIC:
                    attackLevel = static_cast<double>(attacker.GetSkills().GetEffectiveLevel(Skill::MAGIC));
                    break;
                default:
                    attackLevel = 1.0;
                    break;
            }

            int32_t equipBonus = 0;
            switch (type) {
                case CombatType::MELEE:
                    equipBonus = GetMeleeAttackBonus(attacker);
                    break;
                case CombatType::RANGED:
                    equipBonus = GetRangedAttackBonus(attacker);
                    break;
                case CombatType::MAGIC:
                    equipBonus = GetMagicAttackBonus(attacker);
                    break;
                default:
                    break;
            }

            double prayerMod = GetPrayerAttackMultiplier(attacker, type);
            // TODO: Get actual attack style
            AttackStyle style = AttackStyle::MELEE_ACCURATE; // Placeholder
            int32_t styleBonus = GetAttackStyleBonus(style);

            // Formula: (floor(equipBonus + attackLevel) + 8) * prayerMod + styleBonus
            double attackRoll = (std::floor(equipBonus + attackLevel) + 8.0) * prayerMod;
            attackRoll += styleBonus;

            // Apply special attack multiplier if needed
            if (special) {
                // TODO: Apply special accuracy multipliers
            }

            return attackRoll;
        }

        double GetDefenceRoll(const Player& victim, CombatType type) {
            double defenceLevel = static_cast<double>(victim.GetSkills().GetEffectiveLevel(Skill::DEFENCE));

            int32_t equipBonus = 0;
            switch (type) {
                case CombatType::MELEE: {
                    const auto& bonuses = victim.GetEquipmentHandler().GetBonuses();
                    int32_t stab = bonuses.GetBonus(BonusType::STAB_DEFENCE);
                    int32_t slash = bonuses.GetBonus(BonusType::SLASH_DEFENCE);
                    int32_t crush = bonuses.GetBonus(BonusType::CRUSH_DEFENCE);
                    equipBonus = std::max({stab, slash, crush});
                    break;
                }
                case CombatType::RANGED:
                    equipBonus = victim.GetEquipmentHandler().GetBonuses().GetBonus(BonusType::RANGED_DEFENCE);
                    break;
                case CombatType::MAGIC:
                    equipBonus = victim.GetEquipmentHandler().GetBonuses().GetBonus(BonusType::MAGIC_DEFENCE);
                    break;
                default:
                    break;
            }

            double prayerMod = GetPrayerDefenceMultiplier(victim, type);
            // TODO: Get actual defence style
            AttackStyle style = AttackStyle::MELEE_ACCURATE; // Placeholder
            int32_t styleBonus = GetDefenceStyleBonus(style);

            // Formula: (floor(equipBonus + defenceLevel) + 8) * prayerMod + styleBonus
            double defenceRoll = (std::floor(equipBonus + defenceLevel) + 8.0) * prayerMod;
            defenceRoll += styleBonus;

            return defenceRoll;
        }

        double CalculateHitChance(double attackRoll, double defenceRoll) {
            double hitChance;

            if (attackRoll < defenceRoll) {
                // Formula: (A - 1) / (2 * D)
                hitChance = (attackRoll - 1.0) / (2.0 * defenceRoll);
            } else {
                // Formula: 1 - (D + 1) / (2 * A)
                hitChance = 1.0 - ((defenceRoll + 1.0) / (2.0 * attackRoll));
            }

            // Clamp between 0.01 and 0.99
            return std::clamp(hitChance, 0.01, 0.99);
        }

        // ========== Utility Functions ==========

        double GetPrayerAttackMultiplier(const Player& player, CombatType type) {
            // TODO: Implement prayer system and return actual multiplier
            // For now, return 1.0 (no prayer bonus)
            return 1.0;
        }

        double GetPrayerStrengthMultiplier(const Player& player, CombatType type) {
            // TODO: Implement prayer system and return actual multiplier
            // For now, return 1.0 (no prayer bonus)
            return 1.0;
        }

        double GetPrayerDefenceMultiplier(const Player& player, CombatType type) {
            // TODO: Implement prayer system and return actual multiplier
            // For now, return 1.0 (no prayer bonus)
            return 1.0;
        }

        int32_t GetAttackStyleBonus(AttackStyle style) {
            switch (style) {
                case AttackStyle::MELEE_ACCURATE:
                case AttackStyle::RANGE_ACCURATE:
                    return 3;
                case AttackStyle::MELEE_CONTROLLED:
                    return 1;
                default:
                    return 0;
            }
        }

        int32_t GetStrengthStyleBonus(AttackStyle style) {
            switch (style) {
                case AttackStyle::MELEE_AGGRESSIVE:
                    return 3;
                case AttackStyle::MELEE_CONTROLLED:
                    return 1;
                default:
                    return 0;
            }
        }

        int32_t GetDefenceStyleBonus(AttackStyle style) {
            switch (style) {
                case AttackStyle::MELEE_DEFENSIVE:
                case AttackStyle::RANGE_LONGRANGE:
                    return 3;
                case AttackStyle::MELEE_CONTROLLED:
                    return 1;
                default:
                    return 0;
            }
        }

        int32_t GetHighestAttackBonusIndex(const Player& player) {
            const auto& bonuses = player.GetEquipmentHandler().GetBonuses();
            int32_t stab = bonuses.GetBonus(BonusType::STAB_ATTACK);
            int32_t slash = bonuses.GetBonus(BonusType::SLASH_ATTACK);
            int32_t crush = bonuses.GetBonus(BonusType::CRUSH_ATTACK);

            if (slash >= stab && slash >= crush) return 1;
            if (crush >= stab && crush >= slash) return 2;
            return 0;
        }

    }

}
