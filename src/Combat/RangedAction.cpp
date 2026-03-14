#include "RangedAction.h"
#include "../Player.h"
#include "../Position.h"
#include "CombatHandler.h"
#include "CombatFormulas.h"
#include "CombatConstants.h"
#include "Projectile.h"
#include "SpecialAttackData.h"
#include <random>
#include <cmath>

namespace Skeleton {

    bool RangedAction::IsWithinRadius(const Player& attacker, const Player& victim) const {
        int32_t distance = GetAttackDistance(attacker);
        int32_t dx = static_cast<int32_t>(std::abs(static_cast<int>(attacker.GetPosition().GetX() - victim.GetPosition().GetX())));
        int32_t dy = static_cast<int32_t>(std::abs(static_cast<int>(attacker.GetPosition().GetY() - victim.GetPosition().GetY())));
        int32_t maxDist = std::max(dx, dy);

        // Ranged weapons have a maximum range
        return maxDist <= distance;
    }

    void RangedAction::Begin(Player& attacker, Player& victim, bool special) {
        int32_t weaponId = GetWeaponId(attacker);
        int32_t ammoId = 0;

        // Check if player has required ammunition
        if (!HasRequiredAmmo(attacker, weaponId, ammoId)) {
            // TODO: Send message "You don't have enough ammunition"
            return;
        }

        // Check if ammo is compatible with weapon
        if (!IsAmmoCompatible(weaponId, ammoId)) {
            // TODO: Send message about incompatible ammo
            return;
        }

        // Calculate damage
        Hit hit = CalculateDamage(attacker, victim, special);

        // Play attack animation
        int32_t attackAnim = GetAttackAnimation(attacker);
        attacker.PlayAnimation(attackAnim);

        // Display drawback graphic (bow pull animation)
        int32_t drawback = GetDrawbackGraphic(weaponId, ammoId);
        if (drawback != -1) {
            // TODO: Play graphic when graphics system is available
            // attacker.PlayGraphic(drawback, 0, 90);
        }

        // Consume ammunition (unless using certain special weapons like crystal bow)
        int32_t hitCount = special ? GetSpecialHitCount(weaponId) : 1;
        ConsumeAmmo(attacker, ammoId, hitCount);

        // Display projectile
        DisplayProjectile(attacker, victim, weaponId, ammoId);

        // Apply the hit with projectile delay
        Projectile proj = Projectile::CreateRanged(attacker, victim, GetProjectileId(weaponId, ammoId));
        int32_t hitDelay = proj.GetHitDelay();
        ApplyHit(victim, hit, hitDelay);

        // Set attack timer
        int32_t attackSpeed = GetAttackSpeed(attacker);
        // TODO: Set combat handler attack timer when available
        // attacker.GetCombatHandler().SetAttackTimer(attackSpeed);
    }

    Hit RangedAction::CalculateDamage(Player& attacker, Player& victim, bool special) const {
        // Roll accuracy
        bool accurate = CombatFormulas::RollAccuracy(attacker, victim, CombatType::RANGED, special);

        if (!accurate) {
            // Miss - return block hit with 0 damage
            return Hit(std::weak_ptr<Player>(), 0, HitType::BLOCK);
        }

        // Hit - roll damage
        int32_t damage = RollDamage(attacker, victim, special);

        // TODO: Clamp damage to victim's current HP
        // damage = std::min(damage, victim.GetCurrentHitpoints());

        return Hit(std::weak_ptr<Player>(), damage, HitType::NORMAL);
    }

    bool RangedAction::ExecuteSpecialAttack(Player& attacker, Player& victim) {
        int32_t weaponId = GetWeaponId(attacker);

        // Get special attack definition
        const SpecialAttack* special = GetSpecialAttack(weaponId);
        if (!special || !special->IsValid()) {
            return false; // No special attack for this weapon
        }

        // Check if player has enough special energy
        if (!attacker.GetCombatHandler().CanUseSpecial(special->m_EnergyCost)) {
            return false;
        }

        // Check ammunition
        int32_t ammoId = 0;
        if (!HasRequiredAmmo(attacker, weaponId, ammoId)) {
            return false;
        }

        // Play special attack animation
        int32_t specialAnim = special->m_SpecialAnimation > 0 ?
                              special->m_SpecialAnimation : GetAttackAnimation(attacker);
        attacker.PlayAnimation(specialAnim);

        // Display special attack graphic
        if (special->m_SpecialGraphic > 0) {
            // TODO: Play graphic when graphics system is available
            // attacker.PlayGraphic(special->m_SpecialGraphic, 0, 0);
        }

        // Execute the special attack based on hit count
        for (int32_t i = 0; i < special->m_HitCount; i++) {
            Hit hit = CalculateDamage(attacker, victim, true);

            // Display projectile for each hit
            DisplayProjectile(attacker, victim, weaponId, ammoId);

            // Apply hit with delay
            Projectile proj = Projectile::CreateRanged(attacker, victim, GetProjectileId(weaponId, ammoId));
            ApplyHit(victim, hit, proj.GetHitDelay() + i);
        }

        // Consume ammo
        ConsumeAmmo(attacker, ammoId, special->m_HitCount);

        // Consume special energy
        attacker.GetCombatHandler().ConsumeSpecialEnergy(special->m_EnergyCost);

        return true;
    }

    int32_t RangedAction::GetAttackDistance(const Player& player) const {
        int32_t weaponId = GetWeaponId(player);

        // TODO: Get from weapon definition once available
        // For now, use hardcoded values
        switch (weaponId) {
            // Longbows - 10 tile range
            case 839:  // Longbow
            case 845:  // Oak longbow
            case 847:  // Willow longbow
            case 849:  // Maple longbow
            case 851:  // Yew longbow
            case 853:  // Magic longbow
            case 11235: // Dark bow
                return 10;

            // Shortbows, crossbows - 7 tile range
            case 841:  // Shortbow
            case 843:  // Oak shortbow
            case 4734: // Karil's crossbow
            case 9185: // Rune crossbow
                return 7;

            // Thrown weapons - 4 tile range
            case 806:  // Dart
            case 863:  // Knife
            case 825:  // Javelin
                return 4;

            default:
                return 7; // Default range
        }
    }

    int32_t RangedAction::RollDamage(Player& attacker, Player& victim, bool special) const {
        int32_t maxHit = CombatFormulas::CalculateRangedMaxHit(attacker, special);

        // Roll random damage between 0 and maxHit
        static std::random_device rd;
        static std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, maxHit);

        return dis(gen);
    }

    bool RangedAction::HasRequiredAmmo(const Player& player, int32_t weaponId, int32_t& ammoId) const {
        // Check ammunition slot in equipment
        int32_t equippedAmmo = player.GetEquipmentHandler().GetItemInSlot(EquipmentSlot::AMMUNITION);
        if (equippedAmmo > 0 && IsAmmoCompatible(weaponId, equippedAmmo)) {
            ammoId = equippedAmmo;
            return true;
        }

        // Check inventory for compatible ammo (for thrown weapons)
        std::vector<int32_t> compatibleAmmo = GetCompatibleAmmoIds(weaponId);
        int32_t foundAmmo = player.GetInventory().FindCompatibleAmmo(compatibleAmmo);
        if (foundAmmo != -1) {
            ammoId = foundAmmo;
            return true;
        }

        // Check for weapons that don't require ammo (crystal bow, etc.)
        if (IsInfiniteAmmoWeapon(weaponId)) {
            ammoId = GetDefaultProjectileForWeapon(weaponId);
            return true;
        }

        return false;
    }

    bool RangedAction::IsAmmoCompatible(int32_t weaponId, int32_t ammoId) const {
        // TODO: Implement full ammo compatibility checking
        // For now, accept all ammo
        return true;
    }

    void RangedAction::ConsumeAmmo(Player& player, int32_t ammoId, int32_t amount) const {
        int32_t weaponId = GetWeaponId(player);

        // Check if weapon doesn't consume ammo (crystal bow, etc.)
        if (IsInfiniteAmmoWeapon(weaponId)) {
            return;
        }

        // Try to consume from ammunition slot first
        int32_t equippedAmmo = player.GetEquipmentHandler().GetItemInSlot(EquipmentSlot::AMMUNITION);
        if (equippedAmmo == ammoId) {
            // Consume from equipment slot
            // TODO: Implement EquipmentHandler::ConsumeAmmo() when available
            // For now, just remove from inventory if it's there
        }

        // Consume from inventory (for thrown weapons or if ammo is in inventory)
        player.GetInventory().RemoveItem(ammoId, amount);
    }

    int32_t RangedAction::GetProjectileId(int32_t weaponId, int32_t ammoId) const {
        // Arrow projectile IDs
        if (ammoId >= 882 && ammoId <= 892) {
            // Bronze to Dragon arrows
            return 10; // Generic arrow projectile
        }

        // Bolt projectile IDs
        if (ammoId >= 877 && ammoId <= 9244) {
            return 27; // Generic bolt projectile
        }

        // Default projectile
        return 10;
    }

    int32_t RangedAction::GetSpecialEnergyCost(int32_t weaponId) const {
        const SpecialAttack* special = GetSpecialAttack(weaponId);
        return special ? special->m_EnergyCost : 0;
    }

    int32_t RangedAction::GetSpecialHitCount(int32_t weaponId) const {
        const SpecialAttack* special = GetSpecialAttack(weaponId);
        return special ? special->m_HitCount : 1;
    }

    int32_t RangedAction::GetDrawbackGraphic(int32_t weaponId, int32_t ammoId) const {
        // TODO: Implement drawback graphics
        // These are the graphics shown when pulling back a bow
        // Different bows/arrows have different graphics
        return -1; // No graphic for now
    }

    void RangedAction::DisplayProjectile(Player& attacker, Player& victim, int32_t weaponId, int32_t ammoId) const {
        int32_t projectileId = GetProjectileId(weaponId, ammoId);
        Projectile proj = Projectile::CreateRanged(attacker, victim, projectileId);

        // Send projectile to all nearby players
        SendProjectile(attacker.GetPosition(), proj);
    }

    std::vector<int32_t> RangedAction::GetCompatibleAmmoIds(int32_t weaponId) const {
        // TODO: Implement full ammo compatibility database
        // For now, return common ammo types based on weapon type

        // Longbows and shortbows use arrows (882-892)
        if ((weaponId >= 839 && weaponId <= 853) || weaponId == 11235) {
            return {892, 890, 888, 886, 884, 882}; // Dragon to Bronze arrows
        }

        // Crossbows use bolts (877-880, 9139-9244)
        if (weaponId == 9185 || weaponId == 4734) {
            return {9244, 9241, 9240, 877, 878, 879, 880};
        }

        // Thrown weapons (darts, knives, javelins)
        if (weaponId >= 806 && weaponId <= 825) {
            return {weaponId}; // Thrown weapons are their own ammo
        }

        return {};
    }

    bool RangedAction::IsInfiniteAmmoWeapon(int32_t weaponId) const {
        // Crystal bow and similar weapons don't consume ammo
        switch (weaponId) {
            case 4212: // Crystal bow
            case 4214: // Crystal bow (2/10)
            case 4215: // Crystal bow (3/10)
            case 4216: // Crystal bow (4/10)
            case 4217: // Crystal bow (5/10)
            case 4218: // Crystal bow (6/10)
            case 4219: // Crystal bow (7/10)
            case 4220: // Crystal bow (8/10)
            case 4221: // Crystal bow (9/10)
            case 4222: // Crystal bow (10/10)
                return true;
            default:
                return false;
        }
    }

    int32_t RangedAction::GetDefaultProjectileForWeapon(int32_t weaponId) const {
        // Get default projectile for infinite ammo weapons
        if (weaponId >= 4212 && weaponId <= 4222) {
            return 249; // Crystal bow projectile
        }
        return 10; // Default arrow projectile
    }

}
