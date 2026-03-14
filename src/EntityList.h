#pragma once
#include "epch.h"

namespace Skeleton {

template<typename T, size_t Capacity>
class EntityList {
public:
    EntityList() {
        m_Entities.fill(nullptr);
    }

    bool add(std::shared_ptr<T> entity) {
        for (size_t i = 0; i < Capacity; ++i) {
            if (m_Entities[i] == nullptr) {
                m_Entities[i] = entity;
                entity->setIndex(static_cast<int32_t>(i));
                m_Size++;
                return true;
            }
        }
        return false;
    }

    void remove(std::shared_ptr<T> entity) {
        int32_t index = entity->getIndex();
        if (index >= 0 && index < static_cast<int32_t>(Capacity) && m_Entities[index] == entity) {
            m_Entities[index] = nullptr;
            entity->setIndex(-1);
            m_Size--;
        }
    }

    std::shared_ptr<T> get(size_t index) const {
        if (index < Capacity) {
            return m_Entities[index];
        }
        return nullptr;
    }

    bool contains(std::shared_ptr<T> entity) const {
        int32_t index = entity->getIndex();
        return index >= 0 && index < static_cast<int32_t>(Capacity) && m_Entities[index] == entity;
    }

    size_t size() const { return m_Size; }
    size_t capacity() const { return Capacity; }

    class iterator {
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = std::shared_ptr<T>;
        using difference_type = std::ptrdiff_t;
        using pointer = value_type*;
        using reference = value_type&;

        iterator(const std::array<std::shared_ptr<T>, Capacity>* entities, size_t index)
            : m_Entities(entities), m_Index(index) {
            skipNull();
        }

        std::shared_ptr<T> operator*() const {
            return (*m_Entities)[m_Index];
        }

        iterator& operator++() {
            ++m_Index;
            skipNull();
            return *this;
        }

        iterator operator++(int) {
            iterator tmp = *this;
            ++(*this);
            return tmp;
        }

        bool operator==(const iterator& other) const {
            return m_Index == other.m_Index;
        }

        bool operator!=(const iterator& other) const {
            return m_Index != other.m_Index;
        }

    private:
        void skipNull() {
            while (m_Index < Capacity && (*m_Entities)[m_Index] == nullptr) {
                ++m_Index;
            }
        }

        const std::array<std::shared_ptr<T>, Capacity>* m_Entities;
        size_t m_Index;
    };

    iterator begin() const {
        return iterator(&m_Entities, 0);
    }

    iterator end() const {
        return iterator(&m_Entities, Capacity);
    }

private:
    std::array<std::shared_ptr<T>, Capacity> m_Entities;
    size_t m_Size = 0;
};

}
