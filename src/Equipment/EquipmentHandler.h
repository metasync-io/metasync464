#pragma once
#include "EquipmentSlot.h"
#include "EquipmentBonuses.h"
#include <array>
#include <stdint.h>

namespace Skeleton {
    class Player;
}

class EquipmentHandler
{
public:
    EquipmentHandler(Skeleton::Player& player);
    bool Equip(int32_t itemId, EquipmentSlot slot);
    bool Unequip(EquipmentSlot slot);
    int32_t GetItemInSlot(EquipmentSlot slot) const;
    bool IsSlotOccupied(EquipmentSlot slot) const;
    const EquipmentBonuses& GetBonuses() const { return m_Bonuses; }
    EquipmentBonuses& GetBonuses() { return m_Bonuses; }
    const std::array<int32_t, 14>& GetEquipment() const { return m_Equipment; }
    void ClearAll();

private:
    Skeleton::Player& m_Player;
    std::array<int32_t, 14> m_Equipment;
    EquipmentBonuses m_Bonuses;
};
