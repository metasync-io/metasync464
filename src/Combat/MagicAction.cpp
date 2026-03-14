#include "MagicAction.h"
#include "../Player.h"
#include "../Position.h"
#include "CombatHandler.h"
#include "CombatFormulas.h"
#include "CombatConstants.h"
#include "Projectile.h"
#include "../Magic/SpellConstants.h"
#include "../Magic/RuneDefinition.h"
#include <random>
#include <cmath>

namespace Skeleton {

    void MagicAction::SetSpell(int32_t spellId) {
        m_SpellId = spellId;
    }

    const Spell* MagicAction::GetSpell() const {
        if (m_SpellId == -1) {
            return nullptr;
        }
        return GetSpellById(m_SpellId);
    }

    bool MagicAction::IsWithinRadius(const Player& attacker, const Player& victim) const {
        int32_t distance = GetAttackDistance(attacker);
        int32_t dx = static_cast<int32_t>(std::abs(static_cast<int>(attacker.GetPosition().GetX() - victim.GetPosition().GetX())));
        int32_t dy = static_cast<int32_t>(std::abs(static_cast<int>(attacker.GetPosition().GetY() - victim.GetPosition().GetY())));
        int32_t maxDist = std::max(dx, dy);

        // Magic has 10 tile range
        return maxDist <= distance;
    }

    void MagicAction::Begin(Player& attacker, Player& victim, bool special) {
        const Spell* spell = GetSpell();
        if (!spell || !spell->IsValid()) {
            // No spell selected or invalid spell
            // TODO: Send message "Select a spell first"
            return;
        }

        // Check level requirement
        if (!HasRequiredLevel(attacker, *spell)) {
            // TODO: Send message about level requirement
            return;
        }

        // Check rune requirements
        if (!HasRequiredRunes(attacker, *spell)) {
            // TODO: Send message "You don't have enough runes"
            return;
        }

        // Roll accuracy first to determine if spell hits or splashes
        bool accurate = CombatFormulas::RollAccuracy(attacker, victim, CombatType::MAGIC, special);

        // Play cast animation
        int32_t castAnim = spell->m_CastAnimation > 0 ? spell->m_CastAnimation : GetAttackAnimation(attacker);
        attacker.PlayAnimation(castAnim);

        // Display start graphic on caster
        if (spell->m_StartGraphicId > 0) {
            // TODO: Play graphic when graphics system is available
            // attacker.PlayGraphic(spell->m_StartGraphicId, 0, 0);
        }

        // Consume runes
        ConsumeRunes(attacker, *spell);

        if (!accurate) {
            // Spell splashed - show splash graphic and deal no damage
            ApplySplash(attacker, victim);
            Hit splashHit(std::weak_ptr<Player>(), 0, HitType::BLOCK);

            // Apply splash hit with delay
            Projectile proj = Projectile::CreateMagic(attacker, victim, spell->m_ProjectileId);
            int32_t hitDelay = proj.GetHitDelay();
            ApplyHit(victim, splashHit, hitDelay);
        } else {
            // Spell hit - calculate damage
            Hit hit = CalculateDamage(attacker, victim, special);

            // Display spell projectile and end graphic
            DisplaySpellEffects(attacker, victim, *spell);

            // Apply the hit with projectile delay
            Projectile proj = Projectile::CreateMagic(attacker, victim, spell->m_ProjectileId);
            int32_t hitDelay = proj.GetHitDelay();
            ApplyHit(victim, hit, hitDelay);
        }

        // Set attack timer
        int32_t attackSpeed = spell->m_CastSpeed > 0 ? spell->m_CastSpeed : CombatConstants::DEFAULT_ATTACK_SPEED;
        // TODO: Set combat handler attack timer when available
        // attacker.GetCombatHandler().SetAttackTimer(attackSpeed);
    }

    Hit MagicAction::CalculateDamage(Player& attacker, Player& victim, bool special) const {
        const Spell* spell = GetSpell();
        if (!spell || !spell->IsValid()) {
            return Hit(std::weak_ptr<Player>(), 0, HitType::BLOCK);
        }

        // Roll damage based on spell's max hit
        int32_t damage = RollDamage(attacker, victim, *spell, special);

        // TODO: Clamp damage to victim's current HP
        // damage = std::min(damage, victim.GetCurrentHitpoints());

        return Hit(std::weak_ptr<Player>(), damage, HitType::NORMAL);
    }

    bool MagicAction::ExecuteSpecialAttack(Player& attacker, Player& victim) {
        // Most magic spells don't have special attacks
        // Special attacks are handled by special weapons (Staff of the Dead, Toxic Staff, etc.)
        // TODO: Implement weapon-specific magic special attacks
        return false;
    }

    int32_t MagicAction::GetAttackDistance(const Player& player) const {
        // Magic has 10 tile range (same as longbows)
        return 10;
    }

    int32_t MagicAction::RollDamage(Player& attacker, Player& victim, const Spell& spell, bool special) const {
        // Calculate max hit using spell base damage and magic damage bonus
        int32_t maxHit = CombatFormulas::CalculateMagicMaxHit(attacker, spell.m_SpellId);

        // For most spells, use the spell's base max hit as the absolute ceiling
        // Equipment bonuses can increase this but won't go beyond reasonable limits
        int32_t spellMaxHit = spell.m_BaseMaxHit;

        // Apply magic damage bonus from equipment (typically ~20-30% increase at max gear)
        // TODO: Get magic damage bonus from equipment when available
        // For now, maxHit is already calculated in CombatFormulas

        // Clamp to spell's maximum (prevent going too high)
        maxHit = std::min(maxHit, spellMaxHit + (spellMaxHit / 3)); // Max 33% bonus

        // Roll random damage between 0 and maxHit
        static std::random_device rd;
        static std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, maxHit);

        return dis(gen);
    }

    bool MagicAction::HasRequiredRunes(const Player& player, const Spell& spell) const {
        // Check each rune requirement
        for (const auto& rune : spell.m_Runes) {
            if (rune.m_ItemId == -1 || rune.m_Amount == 0) {
                continue; // No requirement for this slot
            }

            // Check if player has infinite runes from staff
            if (HasInfiniteRunes(player, rune.m_ItemId)) {
                continue; // Staff provides this rune
            }

            // Check inventory for rune count
            if (player.GetInventory().GetItemCount(rune.m_ItemId) < rune.m_Amount) {
                return false;
            }
        }

        return true;
    }

    bool MagicAction::HasRequiredLevel(const Player& player, const Spell& spell) const {
        int32_t magicLevel = player.GetSkills().GetLevel(Skill::MAGIC);
        return magicLevel >= spell.m_RequiredLevel;
    }

    void MagicAction::ConsumeRunes(Player& player, const Spell& spell) const {
        // TODO: Implement rune consumption when inventory system is available
        for (const auto& rune : spell.m_Runes) {
            if (rune.m_ItemId == -1 || rune.m_Amount == 0) {
                continue;
            }

            // Skip if player has infinite runes from staff
            if (HasInfiniteRunes(player, rune.m_ItemId)) {
                continue;
            }

            // Remove runes from inventory
            // player.GetInventory().RemoveItem(rune.m_ItemId, rune.m_Amount);
        }
    }

    bool MagicAction::HasInfiniteRunes(const Player& player, int32_t runeId) const {
        // TODO: Check if equipped staff provides infinite runes
        // For example:
        // - Staff of fire provides infinite fire runes
        // - Staff of air provides infinite air runes
        // - Tome of fire provides infinite fire runes
        // - etc.

        // int32_t weaponId = player.GetEquipmentHandler().GetItemInSlot(EquipmentSlot::WEAPON);
        // switch (weaponId) {
        //     case 1387: // Staff of air
        //         return runeId == RuneIds::AIR_RUNE;
        //     case 1393: // Staff of fire
        //         return runeId == RuneIds::FIRE_RUNE;
        //     case 1395: // Staff of water
        //         return runeId == RuneIds::WATER_RUNE;
        //     case 1389: // Staff of earth
        //         return runeId == RuneIds::EARTH_RUNE;
        //     default:
        //         return false;
        // }

        return false;
    }

    void MagicAction::DisplaySpellEffects(Player& attacker, Player& victim, const Spell& spell) const {
        // Display projectile if spell has one
        if (spell.m_ProjectileId > 0) {
            Projectile proj = Projectile::CreateMagic(attacker, victim, spell.m_ProjectileId);
            SendProjectile(attacker.GetPosition(), proj);
        }

        // Display end graphic on victim when projectile arrives
        // This is typically handled by the client based on projectile timing
        // TODO: Schedule end graphic display when graphics system is available
        // ScheduleGraphic(victim, spell.m_EndGraphicId, proj.GetHitDelay());
    }

    void MagicAction::ApplySplash(Player& attacker, Player& victim) const {
        // Splash graphic ID is 85
        const int32_t SPLASH_GRAPHIC = 85;

        // TODO: Display splash graphic on victim
        // This shows the "splash" animation when a spell misses
        // victim.PlayGraphic(SPLASH_GRAPHIC, 0, 100);
    }

}
