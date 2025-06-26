#pragma once

#include <deque>
#include <memory>
#include <cstdint>

#include "Position.h"

namespace Skeleton {
    class Player;

    class MovementHandler 
    {
    public:
        MovementHandler(Player& player);

        void Process();
        void Reset();
        void Finish();
        void AddToPath(const Position& position);

        void SetRunPath(bool runPath) { m_RunPath = runPath; }
        const bool IsRunToggled() { return m_RunToggled; }
        const bool IsRunPath() { return m_RunPath; }

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

        void AddStep(int32_t x, int32_t y);

        Player& m_Player;
        std::deque<std::shared_ptr<Point>> m_Waypoints;
        bool m_RunToggled;
        bool m_RunPath;
    };
}