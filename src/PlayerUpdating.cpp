#include "epch.h"
#include "PlayerUpdating.h"
#include "Constants.h"
#include "World.h"
#include "Util.h"
#include "TextUtils.h"

namespace Skeleton {

	void PlayerUpdating::Update(std::vector<PlayerPtr> players, bool logSummary)
	{
		std::unordered_set<std::string> uniqueIPs;
		size_t totalBytes = 0;
		
		for (const PlayerPtr& player : players)
		{
			UpdatePlayer(player, logSummary);
			
			if (logSummary) {
				try {
					std::string ip = player->GetClient()->m_Socket.remote_endpoint().address().to_string();
					if (ip == "127.0.0.1" || ip == "::1") {
						ip = "localhost";
					}
					uniqueIPs.insert(ip);
				} catch (const std::exception&) {}
			}
		}
		
		if (logSummary && !players.empty()) {
			std::string ipList;
			for (const auto& ip : uniqueIPs) {
				if (!ipList.empty()) ipList += " | ";
				ipList += "\033[96m" + ip + "\033[0m";
			}
			LOG_INFO("[SEND] PlayerUpdate (90) + NPCUpdate (69) [{}]", ipList);
		}
	}

	void PlayerUpdating::UpdatePlayer(PlayerPtr player, bool suppressLog)
	{
		StreamBuffer out(2048);
		StreamBuffer block(1024);

		out.WriteHeader(90, *player->GetClient()->GetEncryptor(), VariableHeaderSize::SHORT);
		out.SetAccessType(AccessType::BIT_ACCESS);

		UpdateLocalPlayerMovement(player, out);
		if (player->NeedsUpdate())
		{
			UpdateState(player, block, false, false);
		}

		out.WriteBits(8, player->GetLocalPlayers().size());
		for (auto it = player->GetLocalPlayers().begin(); it != player->GetLocalPlayers().end(); ) {
			auto& other = *it;
			if (other->GetPosition().IsViewableFrom(player->GetPosition()) &&
				  !other->NeedsUpdate(UpdateFlag::NeedsPlacement))
			{

				UpdateOtherPlayerMovement(other, out);

				if (other->NeedsUpdate()) {
					UpdateState(other, block, false, false);
				}
				++it;
			}
			else {
				out.WriteBit(true);
				out.WriteBits(2, 3);
				it = player->GetLocalPlayers().erase(it);
			}
		}

		auto& worldPlayers = World::Instance().GetPlayers();
		auto& localPlayers = player->GetLocalPlayers();

		if (localPlayers.size() >= 255) {
			return;
		}

		for (auto otherPtr : worldPlayers) {
			if (localPlayers.size() >= 255) {
				break;
			}

			if (!otherPtr || otherPtr == player) {
				continue;
			}
			bool alreadyInList = std::any_of(localPlayers.begin(), localPlayers.end(),
				[&](const std::shared_ptr<Player>& p) {
					return p.get() == otherPtr.get();
				});

			if (!alreadyInList && otherPtr->GetPosition().IsViewableFrom(player->GetPosition())) {
				localPlayers.push_back(otherPtr);
				AddPlayer(out, player, otherPtr);
				UpdateState(otherPtr, block, true, false);
			}
		}

		if (block.Position() > 0)
		{
			out.WriteBits(11, 2047);
			out.SetAccessType(AccessType::BYTE_ACCESS);
			out.WriteBytes(block);
		}
		else {

			out.SetAccessType(AccessType::BYTE_ACCESS);
		}
		out.FinishVariableHeader();

		player->GetClient()->Send(out);
	}

	void PlayerUpdating::UpdateLocalPlayerMovement(PlayerPtr player, StreamBuffer& out)
	{
		bool updateRequired = player->NeedsUpdate();
		if (player->NeedsUpdate(UpdateFlag::NeedsPlacement))
		{
			out.WriteBit(true);
			int32_t posX = player->GetPosition().GetLocalX(player->GetCurrentRegion());
			int32_t posY = player->GetPosition().GetLocalY(player->GetCurrentRegion());
			bool resetMovement = player->NeedsUpdate(UpdateFlag::ResetMovement);
			AppendPlacement(out, posX, posY, player->GetPosition().GetZ(), resetMovement, updateRequired);
		}
		else {
			int32_t pDir = player->GetPrimaryDirection();
			int32_t sDir = player->GetSecondaryDirection();
			if (pDir != -1) {
				out.WriteBit(true);
				if (sDir != -1) {
					AppendRun(out, pDir, sDir, updateRequired);
				}
				else {
					AppendWalk(out, pDir, updateRequired);
				}
			}
			else {
				if (updateRequired) {
					out.WriteBit(true);
					AppendStand(out);
				}
				else {
					out.WriteBit(false);
				}
			}
		}
	}

	void PlayerUpdating::UpdateState(PlayerPtr player, StreamBuffer& block, bool forceAppearance, bool noChat)
	{
		int32_t mask = 0x0;

		if (player->NeedsUpdate(UpdateFlag::FaceCoordinate))
		{
			mask |= 0x4;
		}

		if (player->NeedsUpdate(UpdateFlag::Animation))
		{
			mask |= 0x20;
		}

		if (player->NeedsUpdate(UpdateFlag::Appearance) || forceAppearance)
		{
			mask |= 0x40;
		}

		if (player->NeedsUpdate(UpdateFlag::Chat) && !noChat)
		{
			mask |= 0x1;
			LOG_INFO("[UPDATE STATE] Player '{}' has chat update flag set, mask will include 0x1", player->GetUsername());
		}

		LOG_INFO("[UPDATE STATE] Player '{}' update mask: 0x{:X}, noChat={}", player->GetUsername(), mask, noChat);

		if (mask >= 0x100)
		{
			mask |= 0x10;
			block.WriteByte(mask & 0xFF);
			block.WriteByte(mask >> 8);
		}
		else
		{
			block.WriteByte(mask);
		}

		if (player->NeedsUpdate(UpdateFlag::FaceCoordinate))
		{
			AppendFaceCoordinate(player, block);
		}

		if (player->NeedsUpdate(UpdateFlag::Animation))
		{
			AppendAnimationUpdate(player, block);
		}

		if (player->NeedsUpdate(UpdateFlag::Chat) && !noChat)
		{
			AppendChatUpdate(player, block);
		}

		if (player->NeedsUpdate(UpdateFlag::Appearance) || forceAppearance)
		{
			AppendAppearance(player, block);
		}
	}

	void PlayerUpdating::AppendAnimationUpdate(PlayerPtr player, StreamBuffer& out)
	{
		out.WriteShort(player->GetAnimationId(), ValueType::STANDARD, ByteOrder::LITTLE);
		out.WriteByte(player->GetAnimationDelay(), ValueType::C);
	}

	void PlayerUpdating::AppendFaceCoordinate(PlayerPtr player, StreamBuffer& out)
	{
		// Face coordinate is sent as absolute world position
		// Client will calculate direction based on player position vs face coordinate
		int32_t faceX = player->GetFaceX();
		int32_t faceY = player->GetFaceY();
		out.WriteShort(faceX, ValueType::A, ByteOrder::LITTLE);
		out.WriteShort(faceY, ValueType::STANDARD, ByteOrder::LITTLE);
	}

	void PlayerUpdating::AppendAppearance(PlayerPtr player, StreamBuffer& out)
	{
		StreamBuffer block(128);

		block.WriteByte(player->GetGender());  // 0 = male, 1 = female

		block.WriteByte(-1);
		block.WriteByte(-1);

		if (player->GetNPCCosplayId() == -1)
		{

			if (player->GetEquipment()[EQUIPMENT_SLOT_HEAD] > 1) {
				block.WriteShort(0x200 + player->GetEquipment()[EQUIPMENT_SLOT_HEAD]);
			}
			else {
				block.WriteByte(0);
			}

			if (player->GetEquipment()[EQUIPMENT_SLOT_CAPE] > 1) {
				block.WriteShort(0x200 + player->GetEquipment()[EQUIPMENT_SLOT_CAPE]);
			}
			else {
				block.WriteByte(0);
			}

			if (player->GetEquipment()[EQUIPMENT_SLOT_AMULET] > 1) {
				block.WriteShort(0x200 + player->GetEquipment()[EQUIPMENT_SLOT_AMULET]);
			}
			else {
				block.WriteByte(0);
			}

			if (player->GetEquipment()[EQUIPMENT_SLOT_WEAPON] > 1) {
				block.WriteShort(0x200 + player->GetEquipment()[EQUIPMENT_SLOT_WEAPON]);
			}
			else {
				block.WriteByte(0);
			}

			if (player->GetEquipment()[EQUIPMENT_SLOT_CHEST] > 1) {
				block.WriteShort(0x200 + player->GetEquipment()[EQUIPMENT_SLOT_CHEST]);
			}
			else {
				block.WriteShort(0x100 + player->GetAppearance()[APPEARANCE_SLOT_CHEST]);
			}

			if (player->GetEquipment()[EQUIPMENT_SLOT_SHIELD] > 1) {
				block.WriteShort(0x200 + player->GetEquipment()[EQUIPMENT_SLOT_SHIELD]);
			}
			else {
				block.WriteByte(0);
			}

			if (player->GetEquipment()[EQUIPMENT_SLOT_CHEST] > 1) {
				block.WriteShort(0x200 + player->GetEquipment()[EQUIPMENT_SLOT_CHEST]);
			}
			else {
				block.WriteShort(0x100 + player->GetAppearance()[APPEARANCE_SLOT_ARMS]);
			}

			if (player->GetEquipment()[EQUIPMENT_SLOT_LEGS] > 1) {
				block.WriteShort(0x200 + player->GetEquipment()[EQUIPMENT_SLOT_LEGS]);
			}
			else {
				block.WriteShort(0x100 + player->GetAppearance()[APPEARANCE_SLOT_LEGS]);
			}

			if (false ) {
				block.WriteByte(0);
			}
			else {
				block.WriteShort(0x100 + player->GetAppearance()[APPEARANCE_SLOT_HEAD]);
			}

			if (player->GetEquipment()[EQUIPMENT_SLOT_HANDS] > 1) {
				block.WriteShort(0x200 + player->GetEquipment()[EQUIPMENT_SLOT_HANDS]);
			}
			else {
				block.WriteShort(0x100 + player->GetAppearance()[APPEARANCE_SLOT_HANDS]);
			}

			if (player->GetEquipment()[EQUIPMENT_SLOT_FEET] > 1) {
				block.WriteShort(0x200 + player->GetEquipment()[EQUIPMENT_SLOT_FEET]);
			}
			else {
				block.WriteShort(0x100 + player->GetAppearance()[APPEARANCE_SLOT_FEET]);
			}

			if (false ) {
				block.WriteByte(0);
			}
			else {
				block.WriteShort(0x100 + player->GetAppearance()[APPEARANCE_SLOT_BEARD]);
			}
		}
		else
		{
			block.WriteShort(-1);
			block.WriteShort(player->GetNPCCosplayId());
		}

		block.WriteByte(player->GetColors()[0]);
		block.WriteByte(player->GetColors()[1]);
		block.WriteByte(player->GetColors()[2]);
		block.WriteByte(player->GetColors()[3]);
		block.WriteByte(player->GetColors()[4]);

		// Animation IDs: idle, turn, walk, turn180, turn90CW, turn90CC, run
		// NOTE: Walk animation uses run animation for consistent look
		if (player->Get530AnimationMode()) {
			// 530 revision animations (higher vertex count skeletons)
			// Turn animations kept same as 464
			block.WriteShort(10000);  // idle (530)
			block.WriteShort(0x337);  // turn (823 - same as 464)
			block.WriteShort(10022);  // walk -> uses run anim (530)
			block.WriteShort(0x334);  // turn180 (820 - same as 464)
			block.WriteShort(0x335);  // turn90CW (821 - same as 464)
			block.WriteShort(0x336);  // turn90CC (822 - same as 464)
			block.WriteShort(10022);  // run (530)
		} else {
			// 464 revision animations (default)
			block.WriteShort(0x328);  // 808 = idle
			block.WriteShort(0x337);  // 823 = turn
			block.WriteShort(0x338);  // walk -> uses run anim (824)
			block.WriteShort(0x334);  // 820 = turn180
			block.WriteShort(0x335);  // 821 = turn90CW
			block.WriteShort(0x336);  // 822 = turn90CC
			block.WriteShort(0x338);  // 824 = run
		}

		block.WriteLong(PlayerNameToInt64(player->GetUsername()));
		block.WriteByte(3);
		block.WriteShort(0);

		LOG_INFO("[APPEARANCE] Block size: {} bytes, with ValueType::A will be: {}", block.Position(), block.Position() + 128);

		std::string hexdump;
		for (int i = 0; i < std::min(10, block.Position()); i++) {
			char buf[8];
			snprintf(buf, sizeof(buf), "%d,", static_cast<int8_t>(block.Data()[i]));
			hexdump += buf;
		}
		LOG_INFO("[APPEARANCE] First 10 bytes of block: {}", hexdump);

		LOG_INFO("[APPEARANCE] Position in 'out' before size byte: {}", out.Position());
		out.WriteByte(block.Position(), ValueType::A);
		LOG_INFO("[APPEARANCE] Position in 'out' after size byte: {}", out.Position());
		out.WriteBytes(block);
		LOG_INFO("[APPEARANCE] Position in 'out' after appearance data: {}", out.Position());

	}

	void PlayerUpdating::AppendChatUpdate(PlayerPtr player, StreamBuffer& out)
	{
		ChatMessage* cm = player->GetCurrentChatMessage();
		if (!cm) {
			LOG_WARN("[CHAT UPDATE] No current chat message for player: {}", player->GetUsername());
			return;
		}

		LOG_INFO("[CHAT UPDATE] Writing chat update for '{}': text='{}', colour={}, effect={}",
		         player->GetUsername(), cm->chatText, cm->colour, cm->effect);

		int32_t shortValue = ((cm->colour & 0xFF) << 8) | (cm->effect & 0xFF);
		LOG_INFO("[CHAT UPDATE] Writing short: 0x{:04X} (colour={}, effect={})", shortValue, cm->colour, cm->effect);
		out.WriteShort(shortValue, ValueType::A, ByteOrder::LITTLE);
		out.WriteByte(0, ValueType::A);

		uint8_t chatStr[256] = {0};
		TextUtils::TextPack(chatStr, cm->chatText);
		int32_t offset = chatStr[0] + 1;

		LOG_INFO("[CHAT UPDATE] Packed text length: {}, offset: {}", (int)chatStr[0], offset);

		std::string hexDump;
		for (int i = 0; i < offset && i < 20; i++) {
			char buf[8];
			snprintf(buf, sizeof(buf), "%02X ", chatStr[i]);
			hexDump += buf;
		}
		LOG_INFO("[CHAT UPDATE] Packed bytes (first {} bytes): {}", std::min(offset, 20), hexDump);

		out.WriteByte(offset);
		LOG_INFO("[CHAT UPDATE] Writing offset byte: {}", offset);

		std::string reverseDump;
		for (int i = offset - 1; i >= 0; i--) {
			char buf[8];
			snprintf(buf, sizeof(buf), "%02X ", chatStr[i]);
			reverseDump += buf;
			out.WriteByte(chatStr[i]);
		}
		LOG_INFO("[CHAT UPDATE] Reverse bytes written: {}", reverseDump);

		LOG_INFO("[CHAT UPDATE] Chat block written successfully");
	}

	void PlayerUpdating::AppendPlacement(StreamBuffer& out, int32_t localX, int32_t localY, int32_t z, bool discardMovementQueue, bool attributesUpdate)
	{
		out.WriteBits(2, 3);
		out.WriteBits(2, z);
		out.WriteBits(7, localX);
		out.WriteBit(discardMovementQueue);
		out.WriteBits(7, localY);
		out.WriteBit(attributesUpdate);
	}

	void PlayerUpdating::AppendRun(StreamBuffer& out, int32_t direction, int32_t direction2, bool attributesUpdate)
	{
		out.WriteBits(2, 2);

		out.WriteBits(3, direction);
		out.WriteBits(3, direction2);
		out.WriteBit(attributesUpdate);
	}

	void PlayerUpdating::AppendWalk(StreamBuffer& out, int32_t direction, bool attributesUpdate)
	{
		out.WriteBits(2, 1);

		out.WriteBits(3, direction);
		out.WriteBit(attributesUpdate);
	}

	void PlayerUpdating::UpdateOtherPlayerMovement(PlayerPtr player, StreamBuffer& out)
	{
		bool updateRequired = player->NeedsUpdate();
		int32_t pDir = player->GetPrimaryDirection();
		int32_t sDir = player->GetSecondaryDirection();

		if (pDir != -1) {
			out.WriteBit(true);
			if (sDir != -1) {
				AppendRun(out, pDir, sDir, updateRequired);
			}
			else {
				AppendWalk(out, pDir, updateRequired);
			}
		}
		else {
			if (updateRequired) {
				out.WriteBit(true);
				AppendStand(out);
			}
			else {
				out.WriteBit(false);

			}
		}
	}

	void PlayerUpdating::AppendStand(StreamBuffer& out)
	{
		out.WriteBits(2, 0);
	}

	void PlayerUpdating::AddPlayer(StreamBuffer& out, PlayerPtr player, PlayerPtr other)
	{
		out.WriteBits(11, other->GetIndex());
		int32_t yPos = other->GetPosition().GetY() - player->GetPosition().GetY();
		int32_t xPos = other->GetPosition().GetX() - player->GetPosition().GetX();
		out.WriteBits(5, yPos);
		out.WriteBits(3, 6);
		out.WriteBit(true);
		out.WriteBit(true);
		out.WriteBits(5, xPos);
	}

	void PlayerUpdating::SendFaceUpdate(PlayerPtr targetPlayer, const std::vector<PlayerPtr>& allPlayers)
	{
		// Send face-only update to all players who have targetPlayer in their local list
		// This is used for high-frequency face direction updates (50ms cycle)

		for (const auto& observer : allPlayers) {
			if (!observer || observer == targetPlayer) {
				continue;
			}

			// Check if observer has targetPlayer in their local list
			auto& localPlayers = observer->GetLocalPlayers();
			bool hasTarget = false;
			size_t targetIndex = 0;

			for (size_t i = 0; i < localPlayers.size(); i++) {
				auto it = localPlayers.begin();
				std::advance(it, i);
				if (*it == targetPlayer) {
					hasTarget = true;
					targetIndex = i;
					break;
				}
			}

			if (!hasTarget) {
				continue;  // Observer doesn't have targetPlayer in view
			}

			// Build minimal face-only update packet
			StreamBuffer out(256);
			StreamBuffer block(32);

			out.WriteHeader(90, *observer->GetClient()->GetEncryptor(), VariableHeaderSize::SHORT);
			out.SetAccessType(AccessType::BIT_ACCESS);

			// Local player (observer) - no movement update
			out.WriteBit(false);

			// Other players count
			out.WriteBits(8, localPlayers.size());

			// For each player in observer's local list
			size_t idx = 0;
			for (auto& other : localPlayers) {
				if (other == targetPlayer) {
					// This is our target - send standing with update flag
					out.WriteBit(true);   // Has update
					out.WriteBits(2, 0);  // Standing (no movement)

					// Build update block with face coordinate
					int32_t mask = 0x4;  // Face coordinate mask
					block.WriteByte(mask);
					block.WriteShort(targetPlayer->GetFaceX(), ValueType::A, ByteOrder::LITTLE);
					block.WriteShort(targetPlayer->GetFaceY(), ValueType::STANDARD, ByteOrder::LITTLE);
				} else {
					// Other player - no update
					out.WriteBit(false);
				}
				idx++;
			}

			// Write update block if we have one
			if (block.Position() > 0) {
				out.WriteBits(11, 2047);  // End of player list marker
				out.SetAccessType(AccessType::BYTE_ACCESS);
				out.WriteBytes(block);
			} else {
				out.SetAccessType(AccessType::BYTE_ACCESS);
			}

			out.FinishVariableHeader();
			observer->GetClient()->Send(out);
		}
	}

	void PlayerUpdating::SendMovementUpdate(PlayerPtr targetPlayer, const std::vector<PlayerPtr>& allPlayers,
										   int32_t pDir, int32_t sDir)
	{
		// Send movement-only update to all players who have targetPlayer in their local list
		// This is used for high-frequency movement updates (50ms cycle with timer-based steps)

		// Only send if there's actual movement
		if (pDir == -1) {
			return;
		}

		// CRITICAL: Send movement update to the moving player themselves first
		// Without this, the local client won't see their own movement!
		{
			StreamBuffer out(256);
			out.WriteHeader(90, *targetPlayer->GetClient()->GetEncryptor(), VariableHeaderSize::SHORT);
			out.SetAccessType(AccessType::BIT_ACCESS);

			// Local player movement
			out.WriteBit(true);  // Has update
			if (sDir != -1) {
				// Running - 2 tiles
				out.WriteBits(2, 2);  // Type 2 = run
				out.WriteBits(3, pDir);
				out.WriteBits(3, sDir);
				out.WriteBit(false);  // No update block
			} else {
				// Walking - 1 tile
				out.WriteBits(2, 1);  // Type 1 = walk
				out.WriteBits(3, pDir);
				out.WriteBit(false);  // No update block
			}

			// Other players in local list - no updates for them in this packet
			auto& myLocalPlayers = targetPlayer->GetLocalPlayers();
			out.WriteBits(8, myLocalPlayers.size());
			for (const auto& other : myLocalPlayers) {
				out.WriteBit(false);  // No update for others
			}

			out.SetAccessType(AccessType::BYTE_ACCESS);
			out.FinishVariableHeader();
			targetPlayer->GetClient()->Send(out);
		}

		// Now send to all observers
		for (const auto& observer : allPlayers) {
			if (!observer || observer == targetPlayer) {
				continue;
			}

			// Check if observer has targetPlayer in their local list
			auto& localPlayers = observer->GetLocalPlayers();
			bool hasTarget = false;

			for (const auto& lp : localPlayers) {
				if (lp == targetPlayer) {
					hasTarget = true;
					break;
				}
			}

			if (!hasTarget) {
				continue;  // Observer doesn't have targetPlayer in view
			}

			// Build minimal movement-only update packet
			StreamBuffer out(256);

			out.WriteHeader(90, *observer->GetClient()->GetEncryptor(), VariableHeaderSize::SHORT);
			out.SetAccessType(AccessType::BIT_ACCESS);

			// Local player (observer) - no movement update
			out.WriteBit(false);

			// Other players count
			out.WriteBits(8, localPlayers.size());

			// For each player in observer's local list
			for (const auto& other : localPlayers) {
				if (other == targetPlayer) {
					// This is our target - send movement update
					out.WriteBit(true);   // Has update

					if (sDir != -1) {
						// Running - 2 tiles
						out.WriteBits(2, 2);  // Type 2 = run
						out.WriteBits(3, pDir);
						out.WriteBits(3, sDir);
						out.WriteBit(false);  // No update block
					} else {
						// Walking - 1 tile
						out.WriteBits(2, 1);  // Type 1 = walk
						out.WriteBits(3, pDir);
						out.WriteBit(false);  // No update block
					}
				} else {
					// Other player - no update
					out.WriteBit(false);
				}
			}

			out.SetAccessType(AccessType::BYTE_ACCESS);
			out.FinishVariableHeader();
			observer->GetClient()->Send(out);
		}
	}

	void PlayerUpdating::SendFinePositionUpdate(PlayerPtr targetPlayer, const std::vector<PlayerPtr>& allPlayers)
	{
		// Send fine position update to all players who have targetPlayer in their local list
		// This is a simple broadcast packet (opcode 207) for sub-tile smoothing
		// Format: playerIndex (16-bit), fineX (16-bit), fineZ (16-bit)

		if (!targetPlayer->HasFinePosition()) {
			return;
		}

		int32_t fineX = targetPlayer->GetFineX();
		int32_t fineZ = targetPlayer->GetFineZ();

		for (const auto& observer : allPlayers) {
			if (!observer || observer == targetPlayer) {
				continue;
			}

			// Check if observer has targetPlayer in their local list
			auto& localPlayers = observer->GetLocalPlayers();
			int32_t targetIndex = -1;
			int32_t idx = 0;

			for (const auto& other : localPlayers) {
				if (other == targetPlayer) {
					targetIndex = idx;
					break;
				}
				idx++;
			}

			if (targetIndex == -1) {
				continue;  // Observer doesn't have targetPlayer in view
			}

			// Send simple fine position packet (opcode 207)
			StreamBuffer out(16);
			out.WriteHeader(207, *observer->GetClient()->GetEncryptor(), VariableHeaderSize::NONE);
			out.WriteShort(targetIndex);  // Player index in local list
			out.WriteShort(fineX);        // Fine X position
			out.WriteShort(fineZ);        // Fine Z position
			observer->GetClient()->Send(out);
		}

		// Clear the fine position flag after broadcasting
		targetPlayer->ClearFinePosition();
	}

}
