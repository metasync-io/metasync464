#pragma once
#include <cstdint>

namespace Skeleton {

    /**
     * Represents a single item instance in an inventory slot
     */
    struct Item {
        int32_t m_ItemId;
        int32_t m_Amount;

        Item() : m_ItemId(-1), m_Amount(0) {}

        Item(int32_t itemId, int32_t amount)
            : m_ItemId(itemId), m_Amount(amount) {}

        bool IsEmpty() const {
            return m_ItemId == -1 || m_Amount <= 0;
        }

        bool IsValid() const {
            return m_ItemId > 0 && m_Amount > 0;
        }

        void Clear() {
            m_ItemId = -1;
            m_Amount = 0;
        }

        bool operator==(const Item& other) const {
            return m_ItemId == other.m_ItemId && m_Amount == other.m_Amount;
        }

        bool operator!=(const Item& other) const {
            return !(*this == other);
        }
    };

}
