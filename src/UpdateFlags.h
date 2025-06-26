#pragma once
#include <stdint.h>

enum class UpdateFlag : uint32_t 
{
    None = 0,
    Appearance = 1 << 0,
    Animation = 1 << 1,
    Graphics = 1 << 2,
    Chat = 1 << 3,
    FaceEntity = 1 << 4,
    ForceMove = 1 << 5,
    ResetMovement = 1 << 6,
    NeedsPlacement = 1 << 7,
    // Add more as needed
    All = 0xFFFFFFFF
};

inline UpdateFlag operator|(UpdateFlag a, UpdateFlag b) {
    return static_cast<UpdateFlag>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
}

inline UpdateFlag& operator|=(UpdateFlag& a, UpdateFlag b) {
    a = a | b;
    return a;
}

inline UpdateFlag operator&(UpdateFlag a, UpdateFlag b) {
    return static_cast<UpdateFlag>(static_cast<uint32_t>(a) & static_cast<uint32_t>(b));
}

inline bool hasFlag(UpdateFlag flags, UpdateFlag flag) {
    return static_cast<uint32_t>(flags & flag) != 0;
}