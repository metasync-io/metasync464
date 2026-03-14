#include "EquipmentBonuses.h"
#include "EquipmentHandler.h"
#include "../Items/ItemDefinition.h"

EquipmentBonuses::EquipmentBonuses() : m_Bonuses{0} {}

int32_t EquipmentBonuses::GetBonus(BonusType type) const {
    return m_Bonuses[static_cast<size_t>(type)];
}

void EquipmentBonuses::SetBonus(BonusType type, int32_t value) {
    m_Bonuses[static_cast<size_t>(type)] = value;
}

void EquipmentBonuses::Reset() {
    m_Bonuses.fill(0);
}

void EquipmentBonuses::Recalculate(const EquipmentHandler& equipment) {
    Reset();
    const auto& equippedItems = equipment.GetEquipment();
    for (size_t i = 0; i < equippedItems.size(); i++) {
        int32_t itemId = equippedItems[i];
        if (itemId == -1) continue;

        const ItemDefinition* def = ItemDefinitionManager::Instance().GetDefinition(itemId);
        if (def == nullptr) continue;

        for (size_t bonusIdx = 0; bonusIdx < 13; bonusIdx++) {
            m_Bonuses[bonusIdx] += def->m_EquipmentBonuses[bonusIdx];
        }
    }
}
