#pragma once

#include "Action.h"
#include "Position.h"
#include <memory>

namespace Skeleton {

	class DistancedAction : public Action
	{
	public:
		DistancedAction(Player* player, const Position& target, std::shared_ptr<Action> pendingAction, int64_t cycleTime);

		QueuePolicy GetQueuePolicy() const override { return QueuePolicy::NEVER; }
		WalkablePolicy GetWalkablePolicy() const override { return WalkablePolicy::NON_WALKABLE; }

		void Execute() override;

	private:
		Position m_Target;
		std::shared_ptr<Action> m_PendingAction;
	};

}
