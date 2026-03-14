#include "CombatAction.h"
#include "../Player.h"
#include "../Equipment/EquipmentHandler.h"
#include "../Equipment/EquipmentSlot.h"
#include "CombatConstants.h"
#include "HitQueue.h"

namespace Skeleton {

    bool CombatAction::CanAttack(Player& attacker, Player& victim) const {
        // Check if either is null or destroyed
        // TODO: Add destroyed check when available

        // Check if on same plane/height
        if (attacker.GetPosition().GetZ() != victim.GetPosition().GetZ()) {
            return false;
        }

        // TODO: Add more checks:
        // - Wilderness level checks
        // - Multi-combat checks
        // - Combat level difference checks
        // - Safe zone checks
        // - Already in combat checks

        return true;
    }

    void CombatAction::ApplyHit(Player& victim, const Hit& hit, int32_t delayTicks) {
        victim.GetHitQueue().Add(hit, delayTicks);
    }

    int32_t CombatAction::GetAttackSpeed(const Player& player) const {
        int32_t weaponId = GetWeaponId(player);
        return CombatConstants::GetWeaponSpeed(weaponId);
    }

    int32_t CombatAction::GetAttackAnimation(const Player& player) const {
        int32_t weaponId = GetWeaponId(player);

        // TODO: Implement weapon -> animation mapping
        // For now, return default punch animation
        return 422; // Punch animation
    }

    int32_t CombatAction::GetBlockAnimation(const Player& player) const {
        // TODO: Implement shield/weapon -> block animation mapping
        // For now, return default block animation
        return 404; // Default block animation
    }

    int32_t CombatAction::GetWeaponId(const Player& player) const {
        return player.GetEquipmentHandler().GetItemInSlot(EquipmentSlot::WEAPON);
    }

}
