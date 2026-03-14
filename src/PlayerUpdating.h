#pragma once
#include <vector>
#include <memory>

#include "Player.h"

namespace Skeleton {

	using PlayerPtr = std::shared_ptr<Player>;

	class PlayerUpdating {
	public:
		void Update(std::vector<PlayerPtr> players, bool logSummary = false);

		// Send face-only update for a specific player to all observers (high-frequency updates)
		void SendFaceUpdate(PlayerPtr player, const std::vector<PlayerPtr>& allPlayers);

		// Send movement-only update for a specific player to all observers (high-frequency updates)
		void SendMovementUpdate(PlayerPtr player, const std::vector<PlayerPtr>& allPlayers,
								int32_t primaryDir, int32_t secondaryDir);

		// Send fine position update for a specific player to all observers (sub-tile smoothing)
		void SendFinePositionUpdate(PlayerPtr player, const std::vector<PlayerPtr>& allPlayers);
	private:
		void UpdatePlayer(PlayerPtr player, bool suppressLog = false);
		void UpdateLocalPlayerMovement(PlayerPtr player, StreamBuffer& out);
		void UpdateState(PlayerPtr player, StreamBuffer& block, bool forceAppearance, bool noChat);
		void AppendAnimationUpdate(PlayerPtr player, StreamBuffer& out);
		void AppendFaceCoordinate(PlayerPtr player, StreamBuffer& out);
		void AppendAppearance(PlayerPtr player, StreamBuffer& out);
		void AppendChatUpdate(PlayerPtr player, StreamBuffer& out);
		void AppendPlacement(StreamBuffer& out, int32_t localX, int32_t localY, int32_t z, bool discardMovementQueue, bool attributesUpdate);
		void AppendRun(StreamBuffer& out, int32_t direction, int32_t direction2, bool attributesUpdate);
		void AppendWalk(StreamBuffer& out, int32_t direction, bool attributesUpdate);
		void UpdateOtherPlayerMovement(PlayerPtr player, StreamBuffer& out);
		void AppendStand(StreamBuffer& out);
		void AddPlayer(StreamBuffer& out, PlayerPtr player, PlayerPtr other);
	};

}
