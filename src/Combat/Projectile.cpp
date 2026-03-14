#include "Projectile.h"
#include "../Player.h"
#include "../World.h"
#include <cmath>
#include <algorithm>

namespace Skeleton {

    Projectile Projectile::CreateRanged(Player& attacker, Player& victim, int32_t projectileId) {
        Projectile proj;
        proj.m_ProjectileId = projectileId;
        proj.m_Start = attacker.GetPosition();
        proj.m_End = victim.GetPosition();
        proj.m_StartHeight = 43;
        proj.m_EndHeight = 31;
        proj.m_Angle = 16;

        // Calculate distance
        int32_t dx = static_cast<int32_t>(std::abs(static_cast<int>(attacker.GetPosition().GetX() - victim.GetPosition().GetX())));
        int32_t dy = static_cast<int32_t>(std::abs(static_cast<int>(attacker.GetPosition().GetY() - victim.GetPosition().GetY())));
        int32_t distance = std::max(dx, dy);

        // Calculate speed based on distance (from Java implementation)
        if (distance <= 1) {
            proj.m_Speed = 20;
        } else if (distance <= 3) {
            proj.m_Speed = 25;
        } else if (distance <= 8) {
            proj.m_Speed = 30;
        } else {
            proj.m_Speed = 40;
        }

        // Calculate start speed and hit delay
        int32_t gfxDelay = proj.m_Speed + 20;
        proj.m_StartSpeed = 51;

        return proj;
    }

    Projectile Projectile::CreateMagic(Player& caster, Player& target, int32_t projectileId) {
        Projectile proj;
        proj.m_ProjectileId = projectileId;
        proj.m_Start = caster.GetPosition();
        proj.m_End = target.GetPosition();
        proj.m_StartHeight = 43;
        proj.m_EndHeight = 31;
        proj.m_Angle = 16;

        // Calculate distance
        int32_t dx = static_cast<int32_t>(std::abs(static_cast<int>(caster.GetPosition().GetX() - target.GetPosition().GetX())));
        int32_t dy = static_cast<int32_t>(std::abs(static_cast<int>(caster.GetPosition().GetY() - target.GetPosition().GetY())));
        int32_t distance = std::max(dx, dy);

        // Calculate speed based on distance (from Java implementation)
        if (distance <= 1) {
            proj.m_Speed = 30;
        } else if (distance <= 5) {
            proj.m_Speed = 40;
        } else if (distance <= 8) {
            proj.m_Speed = 45;
        } else {
            proj.m_Speed = 55;
        }

        // Calculate start speed
        int32_t gfxDelay = proj.m_Speed + 20;
        proj.m_StartSpeed = 51;

        return proj;
    }

    int32_t Projectile::GetHitDelay() const {
        int32_t gfxDelay = m_Speed + 20;
        return (gfxDelay / 20) - 2;
    }

    void SendProjectile(const Position& source, const Projectile& projectile) {
        // TODO: Implement packet sending to all nearby players
        // For now, this is a stub
        // This will need to integrate with the packet system
        // Similar to how PlayerUpdating works
    }

    void SendProjectileToPlayer(Player& viewer, const Projectile& projectile) {
        // TODO: Implement packet sending to specific player
        // For now, this is a stub
        // This will need to encode the projectile data into a packet buffer
        // and send it to the player's client
    }

}
