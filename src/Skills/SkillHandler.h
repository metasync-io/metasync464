#pragma once

#include "Skill.h"
#include <array>
#include <cstdint>

namespace Skeleton {

    class Player;

    /**
     * Manages all skills for a player
     */
    class SkillHandler {
    public:
        SkillHandler(Player& player);
        ~SkillHandler() = default;

        /**
         * Gets the current level for a skill (base level, no boost)
         */
        int32_t GetLevel(Skill skill) const;

        /**
         * Gets the effective level for a skill (base + boost)
         */
        int32_t GetEffectiveLevel(Skill skill) const;

        /**
         * Gets the experience for a skill
         */
        int32_t GetExperience(Skill skill) const;

        /**
         * Gets the boost/drain amount for a skill
         */
        int32_t GetBoost(Skill skill) const;

        /**
         * Sets the level for a skill (updates both level and experience)
         */
        void SetLevel(Skill skill, int32_t level);

        /**
         * Adds experience to a skill
         * @return true if leveled up
         */
        bool AddExperience(Skill skill, int32_t amount);

        /**
         * Adds a temporary boost to a skill
         */
        void AddBoost(Skill skill, int32_t amount);

        /**
         * Drains a skill (negative boost)
         */
        void Drain(Skill skill, int32_t amount);

        /**
         * Restores a skill to base level (removes boost/drain)
         */
        void Restore(Skill skill);

        /**
         * Restores all skills to base levels
         */
        void RestoreAll();

        /**
         * Gets the total combat level
         */
        int32_t GetCombatLevel() const;

        /**
         * Gets direct access to skill data (for advanced usage)
         */
        const SkillData& GetSkillData(Skill skill) const;

        /**
         * Gets all skill data
         */
        const std::array<SkillData, static_cast<size_t>(Skill::COUNT)>& GetAllSkills() const {
            return m_Skills;
        }

    private:
        Player& m_Player;
        std::array<SkillData, static_cast<size_t>(Skill::COUNT)> m_Skills;

        /**
         * Validates a skill enum value
         */
        bool IsValidSkill(Skill skill) const;

        /**
         * Updates the player's combat level
         */
        void UpdateCombatLevel();
    };

}
