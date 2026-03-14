#pragma once
#include "Item.h"
#include <array>
#include <vector>
#include <optional>
#include <cstdint>

namespace Skeleton {

    class Player;

    /**
     * Manages a player's inventory
     * Standard OSRS inventory has 28 slots
     */
    class Inventory {
    public:
        static constexpr int32_t INVENTORY_SIZE = 28;

        Inventory(Player& player);
        ~Inventory();

        // ========== Basic Operations ==========

        /**
         * Gets the item in a specific slot
         */
        const Item& GetItem(int32_t slot) const;

        /**
         * Sets an item in a specific slot
         */
        void SetItem(int32_t slot, const Item& item);

        /**
         * Clears a specific slot
         */
        void ClearSlot(int32_t slot);

        /**
         * Clears all slots
         */
        void Clear();

        // ========== Item Queries ==========

        /**
         * Checks if inventory contains at least the specified amount of an item
         */
        bool HasItem(int32_t itemId, int32_t amount = 1) const;

        /**
         * Gets the total count of a specific item across all slots
         */
        int32_t GetItemCount(int32_t itemId) const;

        /**
         * Finds the first slot containing the specified item
         * Returns -1 if not found
         */
        int32_t FindItem(int32_t itemId) const;

        /**
         * Finds all slots containing the specified item
         */
        std::vector<int32_t> FindAllItems(int32_t itemId) const;

        /**
         * Checks if inventory has free slots
         */
        bool HasFreeSlot() const;

        /**
         * Gets the number of free slots
         */
        int32_t GetFreeSlotCount() const;

        /**
         * Finds the first free slot
         * Returns -1 if inventory is full
         */
        int32_t FindFreeSlot() const;

        // ========== Item Manipulation ==========

        /**
         * Adds an item to the inventory
         * Stacks with existing items if stackable
         * Returns true if successful, false if inventory is full
         */
        bool AddItem(int32_t itemId, int32_t amount = 1);

        /**
         * Removes a specific amount of an item from inventory
         * Returns true if successful, false if not enough items
         */
        bool RemoveItem(int32_t itemId, int32_t amount = 1);

        /**
         * Removes item from a specific slot
         * Returns true if successful
         */
        bool RemoveItemFromSlot(int32_t slot, int32_t amount = 1);

        /**
         * Swaps two slots
         */
        void SwapSlots(int32_t slot1, int32_t slot2);

        // ========== Combat-Specific Helpers ==========

        /**
         * Checks if player has any of the specified ammo types
         * Used for ranged combat
         */
        bool HasAmmo(const std::vector<int32_t>& ammoIds, int32_t amount = 1) const;

        /**
         * Finds equipped ammo or ammo in inventory
         * Returns the item ID of the ammo found, or -1 if none
         */
        int32_t FindCompatibleAmmo(const std::vector<int32_t>& ammoIds) const;

        /**
         * Gets all items as a vector (for packet sending)
         */
        std::vector<std::pair<int32_t, int32_t>> GetAllItemsForPacket() const;

        // ========== State ==========

        /**
         * Checks if inventory has been modified and needs update
         */
        bool NeedsUpdate() const { return m_NeedsUpdate; }

        /**
         * Marks inventory as updated
         */
        void ClearUpdateFlag() { m_NeedsUpdate = false; }

    private:
        Player& m_Player;
        std::array<Item, INVENTORY_SIZE> m_Items;
        bool m_NeedsUpdate;

        /**
         * Marks inventory as needing update to client
         */
        void MarkForUpdate();

        /**
         * Checks if an item is stackable
         */
        bool IsStackable(int32_t itemId) const;
    };

}
