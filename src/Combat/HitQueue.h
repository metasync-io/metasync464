#pragma once
#include "Hit.h"
#include <queue>
#include <stdint.h>

namespace Skeleton {
    class Player;
}

class HitQueue
{
public:
    HitQueue(Skeleton::Player& player);
    void Add(const Hit& hit, int32_t delayTicks);
    void Process();
    bool HasPending() const;
    void Clear();
    int64_t GetCurrentTick() const { return m_CurrentTick; }

private:
    struct HitEntry {
        Hit m_Hit;
        int64_t m_ApplyTick;

        HitEntry(const Hit& hit, int64_t applyTick)
            : m_Hit(hit), m_ApplyTick(applyTick) {}

        bool operator<(const HitEntry& other) const {
            return m_ApplyTick > other.m_ApplyTick;
        }
    };

    Skeleton::Player& m_Player;
    std::priority_queue<HitEntry> m_Queue;
    int64_t m_CurrentTick;
};
