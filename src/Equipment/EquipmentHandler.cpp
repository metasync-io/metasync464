#include "EquipmentHandler.h"
#include "../Player.h"
#include "../UpdateFlags.h"

EquipmentHandler::EquipmentHandler(Skeleton::Player& player)
    : m_Player(player), m_Equipment{}, m_Bonuses()
{
    m_Equipment.fill(-1);
}

bool EquipmentHandler::Equip(int32_t itemId, EquipmentSlot slot) {
    if (itemId < 0) return false;
    uint8_t slotIndex = ToIndex(slot);
    m_Equipment[slotIndex] = itemId;
    m_Bonuses.Recalculate(*this);
    m_Player.SetUpdateFlag(UpdateFlag::EquipmentChange);
    return true;
}

bool EquipmentHandler::Unequip(EquipmentSlot slot) {
    uint8_t slotIndex = ToIndex(slot);
    if (m_Equipment[slotIndex] == -1) return false;
    m_Equipment[slotIndex] = -1;
    m_Bonuses.Recalculate(*this);
    m_Player.SetUpdateFlag(UpdateFlag::EquipmentChange);
    return true;
}

int32_t EquipmentHandler::GetItemInSlot(EquipmentSlot slot) const {
    return m_Equipment[ToIndex(slot)];
}

bool EquipmentHandler::IsSlotOccupied(EquipmentSlot slot) const {
    return m_Equipment[ToIndex(slot)] != -1;
}

void EquipmentHandler::ClearAll() {
    m_Equipment.fill(-1);
    m_Bonuses.Reset();
    m_Player.SetUpdateFlag(UpdateFlag::EquipmentChange);
}
