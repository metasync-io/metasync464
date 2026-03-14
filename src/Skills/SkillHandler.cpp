#include "SkillHandler.h"
#include "../Player.h"
#include <algorithm>
#include <cmath>

namespace Skeleton {

    SkillHandler::SkillHandler(Player& player)
        : m_Player(player) {
        // Initialize all skills to level 1 with 0 XP
        for (auto& skill : m_Skills) {
            skill.m_Level = 1;
            skill.m_Experience = 0;
            skill.m_Boost = 0;
        }

        // Hitpoints starts at level 10
        m_Skills[static_cast<size_t>(Skill::HITPOINTS)].m_Level = 10;
        m_Skills[static_cast<size_t>(Skill::HITPOINTS)].m_Experience = GetExperienceForLevel(10);
    }

    bool SkillHandler::IsValidSkill(Skill skill) const {
        return static_cast<size_t>(skill) < static_cast<size_t>(Skill::COUNT);
    }

    int32_t SkillHandler::GetLevel(Skill skill) const {
        if (!IsValidSkill(skill)) return 1;
        return m_Skills[static_cast<size_t>(skill)].m_Level;
    }

    int32_t SkillHandler::GetEffectiveLevel(Skill skill) const {
        if (!IsValidSkill(skill)) return 1;
        return m_Skills[static_cast<size_t>(skill)].GetEffectiveLevel();
    }

    int32_t SkillHandler::GetExperience(Skill skill) const {
        if (!IsValidSkill(skill)) return 0;
        return m_Skills[static_cast<size_t>(skill)].m_Experience;
    }

    int32_t SkillHandler::GetBoost(Skill skill) const {
        if (!IsValidSkill(skill)) return 0;
        return m_Skills[static_cast<size_t>(skill)].m_Boost;
    }

    void SkillHandler::SetLevel(Skill skill, int32_t level) {
        if (!IsValidSkill(skill)) return;

        level = std::clamp(level, 1, 99);
        auto& skillData = m_Skills[static_cast<size_t>(skill)];
        skillData.m_Level = level;
        skillData.m_Experience = GetExperienceForLevel(level);
        skillData.m_Boost = 0;

        // TODO: Send skill update packet to client
        // TODO: Check for level-up interface/message

        UpdateCombatLevel();
    }

    bool SkillHandler::AddExperience(Skill skill, int32_t amount) {
        if (!IsValidSkill(skill) || amount <= 0) return false;

        auto& skillData = m_Skills[static_cast<size_t>(skill)];
        int32_t oldLevel = skillData.m_Level;

        skillData.m_Experience += amount;

        // Check if leveled up
        int32_t newLevel = GetLevelForExperience(skillData.m_Experience);
        if (newLevel > skillData.m_Level) {
            skillData.m_Level = newLevel;

            // Restore boost when leveling up (heal to full HP for hitpoints)
            if (skill == Skill::HITPOINTS) {
                skillData.m_Boost = 0;
            }

            // TODO: Send level-up message/interface
            // TODO: Play level-up graphic
            // TODO: Send skill update packet

            UpdateCombatLevel();
            return true;
        }

        // TODO: Send skill update packet (XP gain only)
        return false;
    }

    void SkillHandler::AddBoost(Skill skill, int32_t amount) {
        if (!IsValidSkill(skill)) return;
        m_Skills[static_cast<size_t>(skill)].AddBoost(amount);

        // TODO: Send skill update packet
    }

    void SkillHandler::Drain(Skill skill, int32_t amount) {
        if (!IsValidSkill(skill)) return;
        m_Skills[static_cast<size_t>(skill)].Drain(amount);

        // TODO: Send skill update packet
    }

    void SkillHandler::Restore(Skill skill) {
        if (!IsValidSkill(skill)) return;
        m_Skills[static_cast<size_t>(skill)].Restore();

        // TODO: Send skill update packet
    }

    void SkillHandler::RestoreAll() {
        for (auto& skill : m_Skills) {
            skill.Restore();
        }

        // TODO: Send skill update packets for all skills
    }

    const SkillData& SkillHandler::GetSkillData(Skill skill) const {
        static SkillData defaultSkill;
        if (!IsValidSkill(skill)) return defaultSkill;
        return m_Skills[static_cast<size_t>(skill)];
    }

    int32_t SkillHandler::GetCombatLevel() const {
        // Combat level formula (OSRS/RS2)
        double attack = GetLevel(Skill::ATTACK);
        double strength = GetLevel(Skill::STRENGTH);
        double defence = GetLevel(Skill::DEFENCE);
        double hitpoints = GetLevel(Skill::HITPOINTS);
        double prayer = GetLevel(Skill::PRAYER);
        double ranged = GetLevel(Skill::RANGED);
        double magic = GetLevel(Skill::MAGIC);

        double base = (defence + hitpoints + std::floor(prayer / 2.0)) / 4.0;

        double melee = (attack + strength) * 0.325;
        double range = std::floor(ranged * 1.5) * 0.325;
        double mage = std::floor(magic * 1.5) * 0.325;

        double combatLevel = base + std::max({melee, range, mage});

        return static_cast<int32_t>(std::floor(combatLevel));
    }

    void SkillHandler::UpdateCombatLevel() {
        // Combat level is calculated on-demand via GetCombatLevel()
        // No need to store it separately
    }

}
