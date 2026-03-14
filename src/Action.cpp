#include "epch.h"
#include "Action.h"
#include "Player.h"
#include "ActionQueue.h"

namespace Skeleton {

	Action::Action(Player* player, int64_t delay, bool immediate)
		: m_Player(player), m_Delay(delay), m_Running(true), m_Immediate(immediate)
	{
	}

	void Action::SetDelay(int64_t delay)
	{
		if (delay < 0) {
			throw std::invalid_argument("Delay must be positive.");
		}
		m_Delay = delay;
	}

	void Action::Stop()
	{
		m_Running = false;
		if (m_Player) {
			m_Player->GetActionQueue().ProcessNextAction();
		}
	}

}
