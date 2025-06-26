#pragma once
#include <vector>
#include <memory>

#include "Player.h"

namespace Skeleton {

	using PlayerPtr = std::shared_ptr<Player>;

	class PlayerUpdating {
	public:
		void Update(std::vector<PlayerPtr> players);
	private:
		void UpdatePlayer(PlayerPtr player);
		void UpdateLocalPlayerMovement(PlayerPtr player, StreamBuffer& out);
		void UpdateState(PlayerPtr player, StreamBuffer& block, bool forceAppearance, bool noChat);
		void AppendAppearance(PlayerPtr player, StreamBuffer& out);
		void AppendPlacement(StreamBuffer& out, int32_t localX, int32_t localY, int32_t z, bool discardMovementQueue, bool attributesUpdate);
		void AppendRun(StreamBuffer& out, int32_t direction, int32_t direction2, bool attributesUpdate);
		void AppendWalk(StreamBuffer& out, int32_t direction, bool attributesUpdate);
		void UpdateOtherPlayerMovement(PlayerPtr player, StreamBuffer& out);
		void AppendStand(StreamBuffer& out);
		void AddPlayer(StreamBuffer& out, PlayerPtr player, PlayerPtr other);
	};

}