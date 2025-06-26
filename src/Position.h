#pragma once
#include "epch.h"

namespace Skeleton {

	class Position
	{
	public:

		Position(uint32_t x = 0, uint32_t y = 0, uint32_t z = 0)
			: x(x), y(y), z(z) {
		}

		// Getters
		uint32_t GetX() const { return x; }
		uint32_t GetY() const { return y; }
		uint32_t GetZ() const { return z; }

		uint32_t GetRegionX() const { return (x >> 3) - 6; }
		uint32_t GetRegionY() const { return (y >> 3) - 6; }

		uint32_t GetLocalX(const Position& base) const { return x - 8 * base.GetRegionX(); }
		uint32_t GetLocalY(const Position& base) const { return y - 8 * base.GetRegionY(); }
		uint32_t GetLocalX() const { return GetLocalX(*this); }
		uint32_t GetLocalY() const { return GetLocalY(*this); }

		void Move(int32_t amountX, int32_t amountY) 
		{
			SetX(GetX() + amountX);
			SetY(GetY() + amountY);
		}

		// Setters
		void SetX(uint32_t value) { x = value; }
		void SetY(uint32_t value) { y = value; }
		void SetZ(uint32_t value) { z = value; }

		static int32_t Direction(int32_t dx, int32_t dy) {
			if (dx < 0) {
				if (dy < 0) return 5;
				if (dy > 0) return 0;
				return 3;
			}
			else if (dx > 0) {
				if (dy < 0) return 7;
				if (dy > 0) return 2;
				return 4;
			}
			else {
				if (dy < 0) return 6;
				if (dy > 0) return 1;
				return -1;
			}
		}

		bool IsViewableFrom(const Position& other) const {
			int32_t deltaX = other.x - x, deltaY = other.y - y;
			return deltaX <= 14 && deltaX >= -15 && deltaY <= 14 && deltaY >= -15;
		}

		static constexpr std::array<int32_t, 8> DIRECTION_DELTA_X{ -1, 0, 1, -1, 1, -1, 0, 1 };
		static constexpr std::array<int32_t, 8> DIRECTION_DELTA_Y{ 1, 1, 1,  0, 0, -1, -1, -1 };
	private:
		uint32_t x, y, z;
	};
}
