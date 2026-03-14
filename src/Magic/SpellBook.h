#pragma once

#include <cstdint>

namespace Skeleton {

    /**
     * Available spellbooks in the game
     */
    enum class SpellBook : uint8_t {
        MODERN = 0,      // Standard spellbook
        ANCIENT = 1,     // Ancient Magicks
        LUNAR = 2,       // Lunar spellbook
        ARCEUUS = 3      // Arceuus spellbook (OSRS)
    };

}
