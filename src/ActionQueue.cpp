#include "epch.h"
#include "ActionQueue.h"

namespace Skeleton {

	void ActionQueue::CancelQueuedActions()
	{
		while (!m_QueuedActions.empty()) {
			auto action = m_QueuedActions.front();
			m_QueuedActions.pop();
			if (action) {
				action->Stop();
			}
		}

		if (m_CurrentAction) {
			m_CurrentAction->Stop();
			m_CurrentAction = nullptr;
		}
	}

	void ActionQueue::AddAction(std::shared_ptr<Action> action)
	{
		if (!action) {
			return;
		}

		if (m_QueuedActions.size() >= MAXIMUM_SIZE) {
			return;
		}

		int32_t queueSize = static_cast<int32_t>(m_QueuedActions.size()) + (m_CurrentAction ? 1 : 0);

		switch (action->GetQueuePolicy()) {
		case Action::QueuePolicy::ALWAYS:
			break;
		case Action::QueuePolicy::NEVER:
			if (queueSize > 0) {
				return;
			}
			break;
		}

		m_QueuedActions.push(action);
		ProcessNextAction();
	}

	void ActionQueue::ClearNonWalkableActions()
	{
		if (m_CurrentAction) {
			switch (m_CurrentAction->GetWalkablePolicy()) {
			case Action::WalkablePolicy::WALKABLE:
				break;
			case Action::WalkablePolicy::NON_WALKABLE:
			case Action::WalkablePolicy::FOLLOW:
				m_CurrentAction->Stop();
				m_CurrentAction = nullptr;
				break;
			}
		}

		std::queue<std::shared_ptr<Action>> newQueue;
		while (!m_QueuedActions.empty()) {
			auto action = m_QueuedActions.front();
			m_QueuedActions.pop();

			if (action) {
				switch (action->GetWalkablePolicy()) {
				case Action::WalkablePolicy::WALKABLE:
					newQueue.push(action);
					break;
				case Action::WalkablePolicy::NON_WALKABLE:
				case Action::WalkablePolicy::FOLLOW:
					action->Stop();
					break;
				}
			}
		}

		m_QueuedActions = std::move(newQueue);
	}

	void ActionQueue::ProcessNextAction()
	{
		if (m_CurrentAction) {
			if (m_CurrentAction->IsRunning()) {
				return;
			}
			else {
				m_CurrentAction = nullptr;
			}
		}

		if (!m_QueuedActions.empty()) {
			m_CurrentAction = m_QueuedActions.front();
			m_QueuedActions.pop();
		}
	}

}
