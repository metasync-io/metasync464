#include "ItemDefinition.h"

ItemDefinitionManager& ItemDefinitionManager::Instance() {
    static ItemDefinitionManager instance;
    return instance;
}

const ItemDefinition* ItemDefinitionManager::GetDefinition(int32_t itemId) const {
    auto it = m_Definitions.find(itemId);
    if (it != m_Definitions.end()) return &it->second;
    return nullptr;
}

void ItemDefinitionManager::RegisterDefinition(const ItemDefinition& definition) {
    m_Definitions[definition.m_ItemId] = definition;
}

void ItemDefinitionManager::InitializeDefaults() {
    // Bronze Dagger (1205)
    ItemDefinition bronzeDagger(1205, "Bronze dagger", true, EquipmentSlot::WEAPON, false);
    bronzeDagger.m_EquipmentBonuses[1] = 6;   // Slash attack
    bronzeDagger.m_EquipmentBonuses[10] = 4;  // Strength
    RegisterDefinition(bronzeDagger);

    // Bronze Sword (1277)
    ItemDefinition bronzeSword(1277, "Bronze sword", true, EquipmentSlot::WEAPON, false);
    bronzeSword.m_EquipmentBonuses[1] = 4;   // Slash attack
    bronzeSword.m_EquipmentBonuses[10] = 5;  // Strength
    RegisterDefinition(bronzeSword);
}
