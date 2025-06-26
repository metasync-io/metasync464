#include "epch.h"
#include "MovementHandler.h"

#include "Player.h"

namespace Skeleton {

	MovementHandler::MovementHandler(Player& player)
		: m_Player(player), m_RunToggled(false), m_RunPath(false) {
	}

    void MovementHandler::Process() 
    {
        std::shared_ptr<Point> walkPoint = nullptr;
        std::shared_ptr<Point> runPoint = nullptr;

        if (!m_Waypoints.empty())
        {
            walkPoint = m_Waypoints.front();
            m_Waypoints.pop_front();
        }

        if (IsRunToggled() || IsRunPath()) 
        {
            if (!m_Waypoints.empty()) {
                runPoint = m_Waypoints.front();
                m_Waypoints.pop_front();
            }
        }

        if (walkPoint && walkPoint->GetDirection() != -1) 
        {
            m_Player.GetPosition().Move(
                Position::DIRECTION_DELTA_X[walkPoint->GetDirection()],
                Position::DIRECTION_DELTA_Y[walkPoint->GetDirection()]
            );
            m_Player.SetPrimaryDirection(walkPoint->GetDirection());
        } 

        if (runPoint && runPoint->GetDirection() != -1)
        {
            m_Player.GetPosition().Move(
                Position::DIRECTION_DELTA_X[runPoint->GetDirection()],
                Position::DIRECTION_DELTA_Y[runPoint->GetDirection()]
            );
            m_Player.SetSecondaryDirection(runPoint->GetDirection());
        }

        int32_t deltaX = m_Player.GetPosition().GetX() - m_Player.GetCurrentRegion().GetRegionX() * 8;
        int32_t deltaY = m_Player.GetPosition().GetY() - m_Player.GetCurrentRegion().GetRegionY() * 8;

        if (deltaX < 16 || deltaX >= 88 || deltaY < 16 || deltaY >= 88)
        {
            m_Player.UpdateMapRegion();
        }
    }

    void MovementHandler::Reset() 
    {
        SetRunPath(false);
        m_Waypoints.clear();
        const Position& p = m_Player.GetPosition();
        m_Waypoints.emplace_back(std::make_shared<Point>(p.GetX(), p.GetY(), -1));
    }

    void MovementHandler::Finish()
    {
        if (!m_Waypoints.empty()) 
        {
            m_Waypoints.pop_front();
        }
    }

    void MovementHandler::AddToPath(const Position& position) 
    {
        if (m_Waypoints.empty())
        {
            Reset();
        }

        const auto& last = m_Waypoints.back();
        int32_t deltaX = position.GetX() - last->GetX();
        int32_t deltaY = position.GetY() - last->GetY();
        int32_t max = std::max(std::abs(deltaX), std::abs(deltaY));

        for (int32_t i = 0; i < max; ++i) {
            if (deltaX < 0) deltaX++;
            else if (deltaX > 0) deltaX--;

            if (deltaY < 0) deltaY++;
            else if (deltaY > 0) deltaY--;

            AddStep(position.GetX() - deltaX, position.GetY() - deltaY);
        }
    }

    void MovementHandler::AddStep(int32_t x, int32_t y)
    {
        if (m_Waypoints.size() >= 100) return;

        const auto& last = m_Waypoints.back();
        int32_t deltaX = x - last->GetX();
        int32_t deltaY = y - last->GetY();
        int32_t direction = Position::Direction(deltaX, deltaY);

        if (direction > -1)
        {
            m_Waypoints.emplace_back(std::make_shared<Point>(x, y, direction));
        }
    }

    // --- Point ---

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