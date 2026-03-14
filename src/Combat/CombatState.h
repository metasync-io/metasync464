#pragma once

#include <cstdint>

namespace Skeleton {

    enum class CombatType : uint8_t {
        NONE = 0,
        MELEE = 1,
        RANGED = 2,
        MAGIC = 3
    };

    enum class AttackStyle : uint8_t {
        // Melee styles
        MELEE_ACCURATE = 0,
        MELEE_AGGRESSIVE = 1,
        MELEE_DEFENSIVE = 2,
        MELEE_CONTROLLED = 3,

        // Ranged styles
        RANGE_ACCURATE = 4,
        RANGE_RAPID = 5,
        RANGE_LONGRANGE = 6,

        // Magic style
        MAGIC_NORMAL = 7
    };

}
