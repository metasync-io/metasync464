#pragma once
#include <array>
#include <stdint.h>

enum class BonusType : uint8_t
{
    STAB_ATTACK = 0,
    SLASH_ATTACK = 1,
    CRUSH_ATTACK = 2,
    MAGIC_ATTACK = 3,
    RANGED_ATTACK = 4,
    STAB_DEFENCE = 5,
    SLASH_DEFENCE = 6,
    CRUSH_DEFENCE = 7,
    MAGIC_DEFENCE = 8,
    RANGED_DEFENCE = 9,
    STRENGTH_BONUS = 10,
    PRAYER_BONUS = 11,
    RANGED_STRENGTH = 12,
    COUNT = 13
};

class EquipmentHandler;

class EquipmentBonuses
{
public:
    EquipmentBonuses();
    int32_t GetBonus(BonusType type) const;
    void SetBonus(BonusType type, int32_t value);
    void Recalculate(const EquipmentHandler& equipment);
    void Reset();
    const std::array<int32_t, 13>& GetBonuses() const { return m_Bonuses; }

private:
    std::array<int32_t, 13> m_Bonuses;
};
