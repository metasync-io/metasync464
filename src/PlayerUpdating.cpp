#include "epch.h"
#include "PlayerUpdating.h"
#include "Constants.h"
#include "World.h"
#include "Util.h"

namespace Skeleton {

	void PlayerUpdating::Update(std::vector<PlayerPtr> players)
	{
		for (const PlayerPtr& player : players)
		{
			UpdatePlayer(player);
		}
	}

	void PlayerUpdating::UpdatePlayer(PlayerPtr player)
	{
		StreamBuffer out(2048);
		StreamBuffer block(1024);

		out.WriteHeader(81, *player->GetClient()->GetEncryptor(), VariableHeaderSize::SHORT);
		out.SetAccessType(AccessType::BIT_ACCESS);

		UpdateLocalPlayerMovement(player, out);
		if (player->NeedsUpdate())
		{
			UpdateState(player, block, false, true);
		}

		out.WriteBits(8, player->GetLocalPlayers().size());
		for (auto it = player->GetLocalPlayers().begin(); it != player->GetLocalPlayers().end(); ) {
			auto& other = *it;
			if (other->GetPosition().IsViewableFrom(player->GetPosition()) &&
				/*other->getStage() == Client::Stage::LOGGED_IN &&*/ !other->NeedsUpdate(UpdateFlag::NeedsPlacement))
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

		// Update the local player list.
		auto& worldPlayers = World::Instance().GetPlayers();
		auto& localPlayers = player->GetLocalPlayers(); // list<shared_ptr<Player>>

		if (localPlayers.size() >= 255) {
			return; // Player limit reached
		}

		for (const auto& [name, otherPtr] : worldPlayers) {
			if (localPlayers.size() >= 255) {
				break;
			}

			if (!otherPtr || otherPtr == player /* || otherPtr->getStage() != Client::Stage::LOGGED_IN */ ) {
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

		// Append the attributes block to the main packet.
		if (block.Position() > 0)
		{
			out.WriteBits(11, 2047); // magic EOF
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
			AppendPlacement(out, posX, posY, player->GetPosition().GetZ(), player->NeedsUpdate(UpdateFlag::ResetMovement), &updateRequired);
		}
		else {
			int32_t pDir = player->GetPrimaryDirection();
			int32_t sDir = player->GetSecondaryDirection();
			if (pDir != -1) { // If they moved.
				out.WriteBit(true); // Yes, there is an update.
				if (sDir != -1) { // If they ran.
					AppendRun(out, pDir, sDir, updateRequired);
				}
				else { // Movement but no running - they walked.
					AppendWalk(out, pDir, updateRequired);
				}
			}
			else { // No movement.
				if (updateRequired) { // Does the state need to be updated?
					out.WriteBit(true); // Yes, there is an update.
					AppendStand(out);
				}
				else { // No update whatsoever.
					out.WriteBit(false);
				}
			}
		}
	}

	void PlayerUpdating::UpdateState(PlayerPtr player, StreamBuffer& block, bool forceAppearance, bool noChat)
	{
		int32_t mask = 0x0;

		if (player->NeedsUpdate(UpdateFlag::Appearance) || forceAppearance)
		{
			mask |= 0x10; // appearance
		}

		if (mask >= 0x100)
		{
			// byte isn't sufficient
			mask |= 0x40; // indication for the client that updateMask is stored in a word
			block.WriteShort(mask, ByteOrder::LITTLE);
		}
		else 
		{
			block.WriteByte(mask);
		}
		if (player->NeedsUpdate(UpdateFlag::Appearance) || forceAppearance)
		{
			AppendAppearance(player, block);
		}
	}

	void PlayerUpdating::AppendAppearance(PlayerPtr player, StreamBuffer& out)
	{
		StreamBuffer block(128);
		block.WriteByte(0); // Gender
		block.WriteByte(0); // Skull icon

		if (player->GetNPCCosplayId() == -1)
		{
			// hat
			if (player->GetEquipment()[EQUIPMENT_SLOT_HEAD] > 1) {
				block.WriteShort(0x200 + player->GetEquipment()[EQUIPMENT_SLOT_HEAD]);
			}
			else {
				block.WriteByte(0);
			}

			// Cape.
			if (player->GetEquipment()[EQUIPMENT_SLOT_CAPE] > 1) {
				block.WriteShort(0x200 + player->GetEquipment()[EQUIPMENT_SLOT_CAPE]);
			}
			else {
				block.WriteByte(0);
			}

			// Amulet.
			if (player->GetEquipment()[EQUIPMENT_SLOT_AMULET] > 1) {
				block.WriteShort(0x200 + player->GetEquipment()[EQUIPMENT_SLOT_AMULET]);
			}
			else {
				block.WriteByte(0);
			}

			// Weapon.
			if (player->GetEquipment()[EQUIPMENT_SLOT_WEAPON] > 1) {
				block.WriteShort(0x200 + player->GetEquipment()[EQUIPMENT_SLOT_WEAPON]);
			}
			else {
				block.WriteByte(0);
			}

			// Chest.
			if (player->GetEquipment()[EQUIPMENT_SLOT_CHEST] > 1) {
				block.WriteShort(0x200 + player->GetEquipment()[EQUIPMENT_SLOT_CHEST]);
			}
			else {
				block.WriteShort(0x100 + player->GetAppearance()[APPEARANCE_SLOT_CHEST]);
			}

			// Shield.
			if (player->GetEquipment()[EQUIPMENT_SLOT_SHIELD] > 1) {
				block.WriteShort(0x200 + player->GetEquipment()[EQUIPMENT_SLOT_SHIELD]);
			}
			else {
				block.WriteByte(0);
			}

			// Arms TODO: Check platebody/non-platebody.
			if (player->GetEquipment()[EQUIPMENT_SLOT_CHEST] > 1) {
				block.WriteShort(0x200 + player->GetEquipment()[EQUIPMENT_SLOT_CHEST]);
			}
			else {
				block.WriteShort(0x100 + player->GetAppearance()[APPEARANCE_SLOT_ARMS]);
			}

			// Legs.
			if (player->GetEquipment()[EQUIPMENT_SLOT_LEGS] > 1) {
				block.WriteShort(0x200 + player->GetEquipment()[EQUIPMENT_SLOT_LEGS]);
			}
			else {
				block.WriteShort(0x100 + player->GetAppearance()[APPEARANCE_SLOT_LEGS]);
			}

			// Head (with a hat already on).
			if (false/*Misc.isFullHelm(e[Misc.EQUIPMENT_SLOT_HEAD]) || Misc.isFullMask(Misc.EQUIPMENT_SLOT_HEAD)*/) {
				block.WriteByte(0);
			}
			else {
				block.WriteShort(0x100 + player->GetAppearance()[APPEARANCE_SLOT_HEAD]);
			}

			// Hands.
			if (player->GetEquipment()[EQUIPMENT_SLOT_HANDS] > 1) {
				block.WriteShort(0x200 + player->GetEquipment()[EQUIPMENT_SLOT_HANDS]);
			}
			else {
				block.WriteShort(0x100 + player->GetAppearance()[APPEARANCE_SLOT_HANDS]);
			}

			// Feet.
			if (player->GetEquipment()[EQUIPMENT_SLOT_FEET] > 1) {
				block.WriteShort(0x200 + player->GetEquipment()[EQUIPMENT_SLOT_FEET]);
			}
			else {
				block.WriteShort(0x100 + player->GetAppearance()[APPEARANCE_SLOT_FEET]);
			}

			// Beard.
			if (false/*Misc.isFullHelm(e[Misc.EQUIPMENT_SLOT_HEAD]) || Misc.isFullMask(Misc.EQUIPMENT_SLOT_HEAD)*/) {
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

		// Player colors
		block.WriteByte(player->GetColors()[0]);
		block.WriteByte(player->GetColors()[1]);
		block.WriteByte(player->GetColors()[2]);
		block.WriteByte(player->GetColors()[3]);
		block.WriteByte(player->GetColors()[4]);

		// Movement animations
		block.WriteShort(0x328); // stand
		block.WriteShort(0x337); // stand turn
		block.WriteShort(0x333); // walk
		block.WriteShort(0x334); // turn 180
		block.WriteShort(0x335); // turn 90 cw
		block.WriteShort(0x336); // turn 90 ccw
		block.WriteShort(0x338); // run

		block.WriteLong(PlayerNameToInt64(player->GetUsername()));
		block.WriteByte(3); // Combat level.
		block.WriteShort(0); // Total level.

		out.WriteByte(block.Position(), ValueType::C); // size of the block
		out.WriteBytes(block);

	}

	void PlayerUpdating::AppendPlacement(StreamBuffer& out, int32_t localX, int32_t localY, int32_t z, bool discardMovementQueue, bool attributesUpdate)
	{
		out.WriteBits(2, 3); // 3 - placement.

		// Append the actual sector.
		out.WriteBits(2, z);
		out.WriteBit(discardMovementQueue);
		out.WriteBit(attributesUpdate);
		out.WriteBits(7, localY);
		out.WriteBits(7, localX);
	}

	void PlayerUpdating::AppendRun(StreamBuffer& out, int32_t direction, int32_t direction2, bool attributesUpdate)
	{
		out.WriteBits(2, 2); // 2 - running.

		// Append the actual sector.
		out.WriteBits(3, direction);
		out.WriteBits(3, direction2);
		out.WriteBit(attributesUpdate);
	}

	void PlayerUpdating::AppendWalk(StreamBuffer& out, int32_t direction, bool attributesUpdate)
	{
		out.WriteBits(2, 1); // 1 - walking.

		// Append the actual sector.
		out.WriteBits(3, direction);
		out.WriteBit(attributesUpdate);
	}

	void PlayerUpdating::UpdateOtherPlayerMovement(PlayerPtr player, StreamBuffer& out)
	{
		bool updateRequired = player->NeedsUpdate();
		int32_t pDir = player->GetPrimaryDirection();
		int32_t sDir = player->GetSecondaryDirection();

		if (pDir != -1) { // If they moved.
			out.WriteBit(true); // Yes, there is an update.
			if (sDir != -1) { // If they ran.
				AppendRun(out, pDir, sDir, updateRequired);
			}
			else { // Movement but no running - they walked.
				AppendWalk(out, pDir, updateRequired);
			}
		}
		else { // No movement.
			if (updateRequired) { // Does the state need to be updated?
				out.WriteBit(true); // Yes, there is an update.
				AppendStand(out);
			}
			else { // No update whatsoever.
				out.WriteBit(false);
				//AppendStand(out);
			}
		}
	}

	void PlayerUpdating::AppendStand(StreamBuffer& out)
	{
		out.WriteBits(2, 0); // 0 - no movement.
	}

	void PlayerUpdating::AddPlayer(StreamBuffer& out, PlayerPtr player, PlayerPtr other)
	{
		out.WriteBits(11, other->GetIndex()); // Server slot.
		out.WriteBit(true); // Yes, an update is required.
		out.WriteBit(true); // Discard walking queue(?)

		// Write the relative position.
		out.WriteBits(5, other->GetPosition().GetY() - player->GetPosition().GetY());
		out.WriteBits(5, other->GetPosition().GetX() - player->GetPosition().GetX());
	}

}