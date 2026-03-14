#pragma once
#include <stdint.h>
#include <memory>

namespace Skeleton {
    class Player;
}

enum class HitType : uint8_t
{
    NORMAL = 0,
    BLOCK = 1,
    POISON = 2,
    DISEASE = 3
};

struct Hit
{
    std::weak_ptr<Skeleton::Player> m_Attacker;
    int32_t m_Damage;
    HitType m_Type;

    Hit() : m_Attacker(), m_Damage(0), m_Type(HitType::NORMAL) {}

    Hit(std::weak_ptr<Skeleton::Player> attacker, int32_t damage, HitType type = HitType::NORMAL)
        : m_Attacker(attacker), m_Damage(damage), m_Type(type) {}

    static Hit Normal(std::weak_ptr<Skeleton::Player> attacker, int32_t damage) {
        return Hit(attacker, damage, HitType::NORMAL);
    }

    static Hit Block(std::weak_ptr<Skeleton::Player> attacker) {
        return Hit(attacker, 0, HitType::BLOCK);
    }
};
