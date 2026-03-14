#pragma once
#include <stdint.h>

enum class EquipmentSlot : uint8_t
{
    HEAD = 0,
    CAPE = 1,
    AMULET = 2,
    WEAPON = 3,
    CHEST = 4,
    SHIELD = 5,
    LEGS = 7,
    HANDS = 9,
    FEET = 10,
    RING = 12,
    AMMUNITION = 13,
    COUNT = 14
};

inline uint8_t ToIndex(EquipmentSlot slot) {
    return static_cast<uint8_t>(slot);
}
