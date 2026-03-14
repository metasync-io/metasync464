#pragma once
#include "WeaponType.h"
#include <stdint.h>

enum class AmmoType : uint8_t
{
    NONE = 0,
    ARROW = 1,
    BOLT = 2,
    THROWN = 3,
    INTERNAL = 4  // Weapons with built-in ammo (crystal bow, etc.)
};

struct WeaponDefinition
{
    int32_t m_ItemId;
    WeaponType m_WeaponType;
    int32_t m_AttackSpeed;
    int32_t m_AttackAnimation;
    int32_t m_BlockAnimation;
    int32_t m_StandAnimation;
    int32_t m_WalkAnimation;
    int32_t m_RunAnimation;
    bool m_IsTwoHanded;
    AmmoType m_AmmoType;
    int32_t m_ProjectileId;
    int32_t m_Range;

    constexpr WeaponDefinition()
        : m_ItemId(-1), m_WeaponType(WeaponType::NONE), m_AttackSpeed(6)
        , m_AttackAnimation(422), m_BlockAnimation(404)
        , m_StandAnimation(808), m_WalkAnimation(819), m_RunAnimation(824)
        , m_IsTwoHanded(false), m_AmmoType(AmmoType::NONE), m_ProjectileId(-1), m_Range(1) {}

    constexpr WeaponDefinition(int32_t itemId, WeaponType type, int32_t speed,
                               int32_t attackAnim, bool twoHanded = false,
                               AmmoType ammoType = AmmoType::NONE, int32_t projectileId = -1, int32_t range = 1)
        : m_ItemId(itemId), m_WeaponType(type), m_AttackSpeed(speed)
        , m_AttackAnimation(attackAnim), m_BlockAnimation(404)
        , m_StandAnimation(808), m_WalkAnimation(819), m_RunAnimation(824)
        , m_IsTwoHanded(twoHanded), m_AmmoType(ammoType), m_ProjectileId(projectileId), m_Range(range) {}
};
