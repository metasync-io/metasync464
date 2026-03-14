#pragma once

#include "CombatAction.h"
#include "Projectile.h"
#include "../Magic/Spell.h"
#include <memory>

namespace Skeleton {

    /**
     * Magic combat action implementation
     * Handles magic attacks with spells from different spellbooks
     */
    class MagicAction : public CombatAction {
    public:
        MagicAction() = default;
        virtual ~MagicAction() = default;

        bool IsWithinRadius(const Player& attacker, const Player& victim) const override;

        void Begin(Player& attacker, Player& victim, bool special = false) override;

        Hit CalculateDamage(Player& attacker, Player& victim, bool special = false) const override;

        bool ExecuteSpecialAttack(Player& attacker, Player& victim) override;

        /**
         * Sets the spell to be cast
         * @param spellId The spell button/interface ID
         */
        void SetSpell(int32_t spellId);

        /**
         * Gets the currently selected spell ID
         */
        int32_t GetSpellId() const { return m_SpellId; }

    private:
        int32_t m_SpellId = -1; // Currently selected spell

        /**
         * Gets the maximum attack distance for magic (10 tiles)
         */
        int32_t GetAttackDistance(const Player& player) const;

        /**
         * Calculates damage for a magic hit
         */
        int32_t RollDamage(Player& attacker, Player& victim, const Spell& spell, bool special) const;

        /**
         * Checks if the player has the required runes for the spell
         */
        bool HasRequiredRunes(const Player& player, const Spell& spell) const;

        /**
         * Checks if the player meets the level requirement for the spell
         */
        bool HasRequiredLevel(const Player& player, const Spell& spell) const;

        /**
         * Consumes runes after casting
         * @param player The player casting the spell
         * @param spell The spell being cast
         */
        void ConsumeRunes(Player& player, const Spell& spell) const;

        /**
         * Checks if the player's equipped staff provides infinite runes
         * @param player The player
         * @param runeId The rune item ID to check
         * @return true if the staff provides infinite runes of this type
         */
        bool HasInfiniteRunes(const Player& player, int32_t runeId) const;

        /**
         * Gets the spell definition for the current spell
         */
        const Spell* GetSpell() const;

        /**
         * Displays the magic projectile and graphics
         */
        void DisplaySpellEffects(Player& attacker, Player& victim, const Spell& spell) const;

        /**
         * Applies splash effect when spell misses
         */
        void ApplySplash(Player& attacker, Player& victim) const;
    };

}
