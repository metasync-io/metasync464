#include "epch.h"
#include "DistancedAction.h"
#include "Player.h"
#include "ActionQueue.h"

namespace Skeleton {

	DistancedAction::DistancedAction(Player* player, const Position& target,
		std::shared_ptr<Action> pendingAction, int64_t cycleTime)
		: Action(player, cycleTime, false), m_Target(target), m_PendingAction(pendingAction)
	{
	}

	void DistancedAction::Execute()
	{
		if (!m_Player) {
			Stop();
			return;
		}

		if (m_Player->GetPosition().IsWithinInteractionDistance(m_Target)
			&& m_Player->GetMovementHandler().IsEmpty()) {
			Stop();
			if (m_PendingAction) {
				m_Player->GetActionQueue().AddAction(m_PendingAction);
			}
		}
	}

}
