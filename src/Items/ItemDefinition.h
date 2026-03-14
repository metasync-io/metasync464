#pragma once
#include "../Equipment/EquipmentSlot.h"
#include <array>
#include <string>
#include <stdint.h>
#include <unordered_map>

struct ItemDefinition
{
    int32_t m_ItemId;
    std::string m_Name;
    bool m_IsEquipable;
    EquipmentSlot m_EquipmentSlot;
    std::array<int32_t, 13> m_EquipmentBonuses;
    bool m_IsTwoHanded;
    int32_t m_Value;

    ItemDefinition()
        : m_ItemId(-1), m_Name(""), m_IsEquipable(false)
        , m_EquipmentSlot(EquipmentSlot::WEAPON)
        , m_EquipmentBonuses{0}, m_IsTwoHanded(false), m_Value(0) {}

    ItemDefinition(int32_t itemId, const std::string& name, bool equipable,
                   EquipmentSlot slot, bool twoHanded = false)
        : m_ItemId(itemId), m_Name(name), m_IsEquipable(equipable)
        , m_EquipmentSlot(slot), m_EquipmentBonuses{0}
        , m_IsTwoHanded(twoHanded), m_Value(0) {}
};

class ItemDefinitionManager
{
public:
    static ItemDefinitionManager& Instance();
    const ItemDefinition* GetDefinition(int32_t itemId) const;
    void RegisterDefinition(const ItemDefinition& definition);
    void InitializeDefaults();

private:
    ItemDefinitionManager() = default;
    std::unordered_map<int32_t, ItemDefinition> m_Definitions;
};
