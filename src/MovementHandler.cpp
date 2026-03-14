#include "epch.h"
#include "MovementHandler.h"

#include "Player.h"

namespace Skeleton {

    MovementHandler::MovementHandler(Player& player)
        : m_Player(player), m_RunToggled(false), m_RunQueue(false),
          m_HasTeleportTarget(false), m_TeleportTarget(0, 0),
          m_LastMovementTime(std::chrono::steady_clock::now()),
          m_MovementProcessedThisCycle(false),
          m_MovementJustCompleted(false),
          m_LastMovementDirection(1) {  // Default facing north (direction 1)
    }

    void MovementHandler::Process()
    {
        // Normal tick - only handle teleporting now
        // Walking/running is handled by ProcessFast() for smoother updates

        if (HasTeleportTarget()) {
            Reset();
            m_Player.GetPosition() = GetTeleportTarget();
            ResetTeleportTarget();
            m_Player.UpdateMapRegion();
            m_Player.SetUpdateFlag(UpdateFlag::NeedsPlacement);
            LOG_INFO("[MOVEMENT] Player teleported to: ({},{})",
                     m_Player.GetPosition().GetX(), m_Player.GetPosition().GetY());
        }
    }

    void MovementHandler::ProcessFast()
    {
        // Fast tick - handle timer-based movement for smoother updates
        // Called every 50ms, but only processes movement when WALK_DELAY_MS has elapsed

        m_MovementProcessedThisCycle = false;
        m_MovementJustCompleted = false;

        // Skip if teleporting (handled by normal tick)
        if (HasTeleportTarget()) {
            return;
        }

        // Check if we have waypoints to process
        if (m_Waypoints.empty()) {
            return;
        }

        // Check if first waypoint has a valid direction (not just the current position marker)
        if (m_Waypoints.front()->GetDirection() == -1) {
            return;
        }

        // Check if enough time has passed since last movement
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
            now - m_LastMovementTime).count();

        if (elapsed < WALK_DELAY_MS) {
            return;  // Not time to move yet
        }

        // Time to process movement!
        m_LastMovementTime = now;

        std::shared_ptr<Point> walkPoint = GetNextPoint();
        std::shared_ptr<Point> runPoint = nullptr;

        if (IsRunning()) {
            runPoint = GetNextPoint();
        }

        int32_t walkDir = walkPoint ? walkPoint->GetDirection() : -1;
        int32_t runDir = runPoint ? runPoint->GetDirection() : -1;
        m_Player.SetPrimaryDirection(walkDir);
        m_Player.SetSecondaryDirection(runDir);

        if (walkPoint || runPoint) {
            m_MovementProcessedThisCycle = true;

            // Track last movement direction for face orientation when stopping
            // Use the final direction (run direction if running, else walk direction)
            if (runDir != -1) {
                m_LastMovementDirection = runDir;
            } else if (walkDir != -1) {
                m_LastMovementDirection = walkDir;
            }

            LOG_DEBUG("[MOVEMENT-FAST] Player {} moved: walk={}, run={}, pos=({},{})",
                     m_Player.GetUsername(), walkDir, runDir,
                     m_Player.GetPosition().GetX(), m_Player.GetPosition().GetY());

            // Check if this was the last movement step (queue now empty or only has position marker)
            bool queueEmpty = m_Waypoints.empty() ||
                              (m_Waypoints.size() == 1 && m_Waypoints.front()->GetDirection() == -1);

            if (queueEmpty) {
                m_MovementJustCompleted = true;
                LOG_DEBUG("[MOVEMENT-FAST] Player {} movement completed, facing direction {}",
                         m_Player.GetUsername(), m_LastMovementDirection);
            }
        }

        // Check for map region change
        int32_t localX = m_Player.GetPosition().GetLocalX(m_Player.GetCurrentRegion());
        int32_t localY = m_Player.GetPosition().GetLocalY(m_Player.GetCurrentRegion());

        if (localX < 16 || localX >= 88 || localY < 16 || localY >= 88) {
            m_Player.UpdateMapRegion();
            LOG_INFO("[MOVEMENT-FAST] Map region change at local ({},{})", localX, localY);
        }
    }

    bool MovementHandler::HasPendingMovement() const
    {
        return m_MovementProcessedThisCycle;
    }

    void MovementHandler::Reset()
    {
        SetRunningQueue(false);
        m_Waypoints.clear();
        const Position& p = m_Player.GetPosition();
        m_Waypoints.emplace_back(std::make_shared<Point>(p.GetX(), p.GetY(), -1));
    }

    void MovementHandler::Finish()
    {

        LOG_INFO("[MOVEMENT] Finish called - queue size BEFORE: {}", m_Waypoints.size());
        if (!m_Waypoints.empty()) {
            auto removed = m_Waypoints.front();
            m_Waypoints.pop_front();
            LOG_INFO("[MOVEMENT] Removed first waypoint: ({}, {}) dir={}",
                     removed->GetX(), removed->GetY(), removed->GetDirection());
        }
        LOG_INFO("[MOVEMENT] Finish called - queue size AFTER: {}", m_Waypoints.size());
    }

    void MovementHandler::AddStep(int32_t x, int32_t y)
    {

        if (m_Waypoints.size() == 0) {

            Reset();
        }

        auto last = m_Waypoints.back();

        int32_t diffX = x - static_cast<int32_t>(last->GetX());
        int32_t diffY = y - static_cast<int32_t>(last->GetY());

        LOG_INFO("[ADDSTEP] Target: ({}, {}), Last: ({}, {}), Diff: ({}, {})",
                 x, y, last->GetX(), last->GetY(), diffX, diffY);

        int32_t max = std::max(std::abs(diffX), std::abs(diffY));
        LOG_INFO("[ADDSTEP] Will interpolate {} steps", max);

        for (int32_t i = 0; i < max; i++) {

            if (diffX < 0) {
                diffX++;
            } else if (diffX > 0) {
                diffX--;
            }
            if (diffY < 0) {
                diffY++;
            } else if (diffY > 0) {
                diffY--;
            }

            int32_t nextX = x - diffX;
            int32_t nextY = y - diffY;
            LOG_INFO("[ADDSTEP] Interpolated step {}: ({}, {})", i, nextX, nextY);
            AddStepInternal(nextX, nextY);
        }
    }

    void MovementHandler::AddStepInternal(int32_t x, int32_t y)
    {

        if (m_Waypoints.size() >= MAXIMUM_SIZE) {

            return;
        }

        auto last = m_Waypoints.back();

        int32_t diffX = x - static_cast<int32_t>(last->GetX());
        int32_t diffY = y - static_cast<int32_t>(last->GetY());

        int32_t dir = Position::Direction(diffX, diffY);

        if (dir > -1) {

            m_Waypoints.emplace_back(std::make_shared<Point>(x, y, dir));

        }
    }

    std::shared_ptr<MovementHandler::Point> MovementHandler::GetNextPoint()
    {

        if (m_Waypoints.empty()) {
            return nullptr;
        }

        auto p = m_Waypoints.front();
        m_Waypoints.pop_front();

        if (!p || p->GetDirection() == -1) {
            return nullptr;
        } else {

            int32_t diffX = Position::DIRECTION_DELTA_X[p->GetDirection()];
            int32_t diffY = Position::DIRECTION_DELTA_Y[p->GetDirection()];
            m_Player.GetPosition().Move(diffX, diffY);

            LOG_INFO("[MOVEMENT] Moving to ({},{}) with direction {}",
                     m_Player.GetPosition().GetX(), m_Player.GetPosition().GetY(), p->GetDirection());

            return p;
        }
    }

    void MovementHandler::SetTeleportTarget(const Position& target)
    {
        m_TeleportTarget = target;
        m_HasTeleportTarget = true;
    }

    void MovementHandler::ResetTeleportTarget()
    {
        m_HasTeleportTarget = false;
        m_TeleportTarget = Position(0, 0);
    }

    MovementHandler::Point::Point(int32_t x, int32_t y, int32_t direction)
        : Position(x, y), m_Direction(direction) {
    }

    int32_t MovementHandler::Point::GetDirection() const
    {
        return m_Direction;
    }

    void MovementHandler::Point::SetDirection(int32_t direction)
    {
        m_Direction = direction;
    }

}
