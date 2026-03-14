#pragma once

#include <queue>
#include <memory>
#include "Action.h"

namespace Skeleton {

	class ActionQueue
	{
	public:

		static const int32_t MAXIMUM_SIZE = 28;

		ActionQueue() = default;

		void CancelQueuedActions();

		void AddAction(std::shared_ptr<Action> action);

		void ClearNonWalkableActions();

		void ProcessNextAction();

		std::shared_ptr<Action> GetCurrentAction() const { return m_CurrentAction; }

	private:

		std::queue<std::shared_ptr<Action>> m_QueuedActions;

		std::shared_ptr<Action> m_CurrentAction = nullptr;
	};

}
