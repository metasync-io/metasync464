#include "epch.h"
#include "Inventory.h"
#include "../Player.h"
#include "ItemDefinition.h"
#include <algorithm>

namespace Skeleton {

    Inventory::Inventory(Player& player)
        : m_Player(player), m_NeedsUpdate(false) {
        Clear();
    }

    Inventory::~Inventory() = default;

    // ========== Basic Operations ==========

    const Item& Inventory::GetItem(int32_t slot) const {
        if (slot < 0 || slot >= INVENTORY_SIZE) {
            static Item empty;
            return empty;
        }
        return m_Items[slot];
    }

    void Inventory::SetItem(int32_t slot, const Item& item) {
        if (slot < 0 || slot >= INVENTORY_SIZE) return;
        m_Items[slot] = item;
        MarkForUpdate();
    }

    void Inventory::ClearSlot(int32_t slot) {
        if (slot < 0 || slot >= INVENTORY_SIZE) return;
        m_Items[slot].Clear();
        MarkForUpdate();
    }

    void Inventory::Clear() {
        for (auto& item : m_Items) {
            item.Clear();
        }
        MarkForUpdate();
    }

    // ========== Item Queries ==========

    bool Inventory::HasItem(int32_t itemId, int32_t amount) const {
        return GetItemCount(itemId) >= amount;
    }

    int32_t Inventory::GetItemCount(int32_t itemId) const {
        int32_t count = 0;
        for (const auto& item : m_Items) {
            if (item.m_ItemId == itemId) {
                count += item.m_Amount;
            }
        }
        return count;
    }

    int32_t Inventory::FindItem(int32_t itemId) const {
        for (int32_t i = 0; i < INVENTORY_SIZE; i++) {
            if (m_Items[i].m_ItemId == itemId && m_Items[i].m_Amount > 0) {
                return i;
            }
        }
        return -1;
    }

    std::vector<int32_t> Inventory::FindAllItems(int32_t itemId) const {
        std::vector<int32_t> slots;
        for (int32_t i = 0; i < INVENTORY_SIZE; i++) {
            if (m_Items[i].m_ItemId == itemId && m_Items[i].m_Amount > 0) {
                slots.push_back(i);
            }
        }
        return slots;
    }

    bool Inventory::HasFreeSlot() const {
        return FindFreeSlot() != -1;
    }

    int32_t Inventory::GetFreeSlotCount() const {
        int32_t count = 0;
        for (const auto& item : m_Items) {
            if (item.IsEmpty()) {
                count++;
            }
        }
        return count;
    }

    int32_t Inventory::FindFreeSlot() const {
        for (int32_t i = 0; i < INVENTORY_SIZE; i++) {
            if (m_Items[i].IsEmpty()) {
                return i;
            }
        }
        return -1;
    }

    // ========== Item Manipulation ==========

    bool Inventory::AddItem(int32_t itemId, int32_t amount) {
        if (amount <= 0) return false;

        // Check if item is stackable
        if (IsStackable(itemId)) {
            // Find existing stack
            int32_t slot = FindItem(itemId);
            if (slot != -1) {
                m_Items[slot].m_Amount += amount;
                MarkForUpdate();
                return true;
            }
        }

        // Add to free slots
        int32_t remaining = amount;
        while (remaining > 0) {
            int32_t freeSlot = FindFreeSlot();
            if (freeSlot == -1) {
                return false; // Inventory full
            }

            if (IsStackable(itemId)) {
                m_Items[freeSlot] = Item(itemId, remaining);
                remaining = 0;
            } else {
                m_Items[freeSlot] = Item(itemId, 1);
                remaining--;
            }
        }

        MarkForUpdate();
        return true;
    }

    bool Inventory::RemoveItem(int32_t itemId, int32_t amount) {
        if (amount <= 0) return false;

        // Check if we have enough
        if (!HasItem(itemId, amount)) {
            return false;
        }

        // Remove from slots
        int32_t remaining = amount;
        for (int32_t i = 0; i < INVENTORY_SIZE && remaining > 0; i++) {
            if (m_Items[i].m_ItemId == itemId) {
                int32_t removeAmount = std::min(remaining, m_Items[i].m_Amount);
                m_Items[i].m_Amount -= removeAmount;
                remaining -= removeAmount;

                if (m_Items[i].m_Amount <= 0) {
                    m_Items[i].Clear();
                }
            }
        }

        MarkForUpdate();
        return true;
    }

    bool Inventory::RemoveItemFromSlot(int32_t slot, int32_t amount) {
        if (slot < 0 || slot >= INVENTORY_SIZE) return false;
        if (m_Items[slot].IsEmpty()) return false;
        if (amount <= 0) return false;

        if (m_Items[slot].m_Amount >= amount) {
            m_Items[slot].m_Amount -= amount;
            if (m_Items[slot].m_Amount <= 0) {
                m_Items[slot].Clear();
            }
            MarkForUpdate();
            return true;
        }

        return false;
    }

    void Inventory::SwapSlots(int32_t slot1, int32_t slot2) {
        if (slot1 < 0 || slot1 >= INVENTORY_SIZE) return;
        if (slot2 < 0 || slot2 >= INVENTORY_SIZE) return;

        std::swap(m_Items[slot1], m_Items[slot2]);
        MarkForUpdate();
    }

    // ========== Combat-Specific Helpers ==========

    bool Inventory::HasAmmo(const std::vector<int32_t>& ammoIds, int32_t amount) const {
        for (int32_t ammoId : ammoIds) {
            if (HasItem(ammoId, amount)) {
                return true;
            }
        }
        return false;
    }

    int32_t Inventory::FindCompatibleAmmo(const std::vector<int32_t>& ammoIds) const {
        for (int32_t ammoId : ammoIds) {
            if (HasItem(ammoId, 1)) {
                return ammoId;
            }
        }
        return -1;
    }

    std::vector<std::pair<int32_t, int32_t>> Inventory::GetAllItemsForPacket() const {
        std::vector<std::pair<int32_t, int32_t>> items;
        items.reserve(INVENTORY_SIZE);

        for (const auto& item : m_Items) {
            if (item.IsEmpty()) {
                items.emplace_back(0, 0);
            } else {
                items.emplace_back(item.m_ItemId, item.m_Amount);
            }
        }

        return items;
    }

    // ========== Private Helpers ==========

    void Inventory::MarkForUpdate() {
        m_NeedsUpdate = true;
    }

    bool Inventory::IsStackable(int32_t itemId) const {
        // Common stackable items in OSRS:
        // - Coins (995)
        // - All runes (554-566, 9075)
        // - Arrows, bolts, darts, etc.
        // - Stackable consumables

        // For now, we'll use a simple heuristic:
        // - Coins are always stackable
        if (itemId == 995) return true;

        // - Runes are stackable (IDs 554-566, 9075)
        if ((itemId >= 554 && itemId <= 566) || itemId == 9075) return true;

        // - Arrows (typically IDs 882-892, 4740, 4166-4174, 11212, etc.)
        // - Bolts (typically IDs 877-880, 9139-9144, etc.)
        // For simplicity, we'll check the item definition later
        // For now, return false for non-rune/non-coin items

        // TODO: Check ItemDefinition for stackable flag when implemented
        const ItemDefinition* def = ItemDefinitionManager::Instance().GetDefinition(itemId);
        if (def) {
            // Add stackable flag to ItemDefinition if needed
            // For now, assume non-equipable items might be stackable
        }

        return false;
    }

}
