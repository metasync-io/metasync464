#pragma once

#include <deque>
#include <memory>
#include <cstdint>
#include <chrono>

#include "Position.h"

namespace Skeleton {
    class Player;

    class MovementHandler
    {
    public:
        static const int32_t MAXIMUM_SIZE = 50;
        static const int32_t WALK_DELAY_MS = 600;   // Time between walk steps
        static const int32_t RUN_DELAY_MS = 600;    // Time for 2 run steps (same tick)

        MovementHandler(Player& player);

        void Process();           // Called from normal 600ms tick (now mostly for teleport)
        void ProcessFast();       // Called from fast 50ms tick for movement
        bool HasPendingMovement() const;  // Check if movement update should be sent
        bool HasMovementJustCompleted() const { return m_MovementJustCompleted; }
        int32_t GetLastMovementDirection() const { return m_LastMovementDirection; }
        void Reset();
        void Finish();
        void AddStep(int32_t x, int32_t y);
        bool IsEmpty() const { return m_Waypoints.empty(); }
        size_t GetQueueSize() const { return m_Waypoints.size(); }

        void SetRunningToggled(bool runToggled) { m_RunToggled = runToggled; }
        void SetRunningQueue(bool runQueue) { m_RunQueue = runQueue; }
        bool IsRunningToggled() const { return m_RunToggled; }
        bool IsRunningQueue() const { return m_RunQueue; }
        bool IsRunning() const { return m_RunToggled || m_RunQueue; }

        bool HasTeleportTarget() const { return m_HasTeleportTarget; }
        void SetTeleportTarget(const Position& target);
        const Position& GetTeleportTarget() const { return m_TeleportTarget; }
        void ResetTeleportTarget();

    private:
        class Point : public Position
        {
        public:
            Point(int32_t x, int32_t y, int32_t direction);
            int32_t GetDirection() const;
            void SetDirection(int32_t direction);

        private:
            int32_t m_Direction;
        };

        void AddStepInternal(int32_t x, int32_t y);
        std::shared_ptr<Point> GetNextPoint();

        Player& m_Player;
        std::deque<std::shared_ptr<Point>> m_Waypoints;

        bool m_RunToggled;
        bool m_RunQueue;

        bool m_HasTeleportTarget;
        Position m_TeleportTarget;

        // Timer-based movement tracking
        std::chrono::steady_clock::time_point m_LastMovementTime;
        bool m_MovementProcessedThisCycle = false;  // Flag for fast update broadcast
        bool m_MovementJustCompleted = false;       // Flag for face direction on stop
        int32_t m_LastMovementDirection = 1;        // Last walk direction (default north = 1)
    };
}
