#pragma once

#include <cstdint>
#include <memory>

namespace Skeleton {

	class Player;
	class ActionQueue;

	class Action
	{
	public:

		enum class QueuePolicy {
			ALWAYS,
			NEVER
		};

		enum class WalkablePolicy {
			WALKABLE,
			NON_WALKABLE,
			FOLLOW
		};

		Action(Player* player, int64_t delay, bool immediate);

		virtual ~Action() = default;

		Player* GetPlayer() const { return m_Player; }
		int64_t GetDelay() const { return m_Delay; }
		bool IsRunning() const { return m_Running; }
		bool IsImmediate() const { return m_Immediate; }

		void SetDelay(int64_t delay);

		virtual void Stop();

		virtual void Execute() = 0;

		virtual QueuePolicy GetQueuePolicy() const = 0;

		virtual WalkablePolicy GetWalkablePolicy() const = 0;

	protected:

		Player* m_Player;
		int64_t m_Delay;
		bool m_Running;
		bool m_Immediate;
	};

}
