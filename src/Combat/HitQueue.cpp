#include "HitQueue.h"
#include "../Player.h"
#include "../UpdateFlags.h"

HitQueue::HitQueue(Skeleton::Player& player)
    : m_Player(player), m_Queue(), m_CurrentTick(0) {}

void HitQueue::Add(const Hit& hit, int32_t delayTicks) {
    int64_t applyTick = m_CurrentTick + delayTicks;
    m_Queue.emplace(hit, applyTick);
}

void HitQueue::Process() {
    m_CurrentTick++;

    while (!m_Queue.empty()) {
        const HitEntry& entry = m_Queue.top();
        if (entry.m_ApplyTick > m_CurrentTick) break;

        // Apply damage to player
        const Hit& hit = entry.m_Hit;
        if (hit.m_Damage > 0) {
            m_Player.Damage(hit.m_Damage);
        }

        m_Player.SetUpdateFlag(UpdateFlag::Hit);
        m_Queue.pop();
    }
}

bool HitQueue::HasPending() const {
    return !m_Queue.empty();
}

void HitQueue::Clear() {
    while (!m_Queue.empty()) {
        m_Queue.pop();
    }
}
