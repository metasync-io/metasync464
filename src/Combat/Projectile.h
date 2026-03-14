#pragma once

#include <cstdint>
#include <memory>
#include "../Position.h"

namespace Skeleton {

    class Player;

    /**
     * Represents a projectile in the game world
     * Projectiles are visual-only and sent via packets to clients
     */
    struct Projectile {
        int32_t m_ProjectileId;      // The graphics ID of the projectile
        Position m_Start;             // Starting position
        Position m_End;               // Ending position
        int32_t m_StartHeight;        // Starting height (0-255)
        int32_t m_EndHeight;          // Ending height (0-255)
        int32_t m_StartSpeed;         // Delay before projectile starts (client ticks)
        int32_t m_Speed;              // Speed of projectile travel
        int32_t m_Angle;              // Angle of projectile arc
        int32_t m_Slope;              // Slope for multi-target projectiles
        int32_t m_Radius;             // Radius for area effect projectiles
        std::weak_ptr<Player> m_LockOn; // Target to lock onto (optional)

        /**
         * Default constructor
         */
        Projectile()
            : m_ProjectileId(0), m_Start(), m_End(), m_StartHeight(43),
              m_EndHeight(31), m_StartSpeed(51), m_Speed(70), m_Angle(16),
              m_Slope(0), m_Radius(0) {}

        /**
         * Creates a basic projectile
         * @param start Starting position
         * @param end Ending position
         * @param projectileId Graphics ID
         * @param startHeight Starting height
         * @param endHeight Ending height
         * @param startSpeed Delay before projectile starts
         * @param speed Travel speed
         */
        Projectile(const Position& start, const Position& end, int32_t projectileId,
                   int32_t startHeight, int32_t endHeight, int32_t startSpeed, int32_t speed)
            : m_ProjectileId(projectileId), m_Start(start), m_End(end),
              m_StartHeight(startHeight), m_EndHeight(endHeight),
              m_StartSpeed(startSpeed), m_Speed(speed), m_Angle(16),
              m_Slope(0), m_Radius(0) {}

        /**
         * Creates a projectile with lock-on target
         */
        Projectile(const Position& start, std::weak_ptr<Player> lockOn, int32_t projectileId,
                   int32_t startHeight, int32_t endHeight, int32_t startSpeed, int32_t speed,
                   int32_t angle)
            : m_ProjectileId(projectileId), m_Start(start), m_End(),
              m_StartHeight(startHeight), m_EndHeight(endHeight),
              m_StartSpeed(startSpeed), m_Speed(speed), m_Angle(angle),
              m_Slope(0), m_Radius(0), m_LockOn(lockOn) {
            // End position will be set when lock-on target is available
        }

        /**
         * Creates a projectile for ranged combat
         * Automatically calculates speed and hit delay based on distance
         * @param attacker The attacking player
         * @param victim The target player
         * @param projectileId Graphics ID
         * @return Projectile configured for ranged combat
         */
        static Projectile CreateRanged(Player& attacker, Player& victim, int32_t projectileId);

        /**
         * Creates a projectile for magic combat
         * Automatically calculates speed and hit delay based on distance
         * @param caster The casting player
         * @param target The target player
         * @param projectileId Graphics ID
         * @return Projectile configured for magic combat
         */
        static Projectile CreateMagic(Player& caster, Player& target, int32_t projectileId);

        /**
         * Gets the calculated hit delay in game ticks
         * Based on speed and distance
         */
        int32_t GetHitDelay() const;
    };

    /**
     * Sends a projectile packet to all players who can see it
     * @param source The source position (usually the attacker)
     * @param projectile The projectile to send
     */
    void SendProjectile(const Position& source, const Projectile& projectile);

    /**
     * Sends a projectile packet to a specific player
     * @param viewer The player who will see the projectile
     * @param projectile The projectile to send
     */
    void SendProjectileToPlayer(Player& viewer, const Projectile& projectile);

}
