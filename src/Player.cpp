#include "epch.h"
#include "Player.h"
#include "World.h"

namespace Skeleton {
     
    Player::Player(const std::string& username, std::weak_ptr<Client> client)
        : m_Username(username), m_Client(client), m_Position(3200, 3200), m_MovementHandler(std::make_unique<MovementHandler>(*this))
    {
        m_Equipment.fill(0);
    }

    const std::string& Player::GetUsername() const
    {
        return m_Username;
    }

    void Player::Logout()
    {
        LOG_INFO("Player logout");
    }

    void Player::Process()
    {
        m_MovementHandler->Process();
    }

    void Player::Reset()
    {
        SetPrimaryDirection(-1);
        SetSecondaryDirection(-1);
        ClearUpdateFlags();
    }

    void Player::Login()
    {
        UpdateMapRegion();
        SetUpdateFlag(UpdateFlag::All);
        GetClient()->SendSidebarInterface(1, 3917);
        GetClient()->SendSidebarInterface(2, 638);
        GetClient()->SendSidebarInterface(3, 3213);
        GetClient()->SendSidebarInterface(4, 1644);
        GetClient()->SendSidebarInterface(5, 5608);
        GetClient()->SendSidebarInterface(6, 1151);
        GetClient()->SendSidebarInterface(8, 5065);
        GetClient()->SendSidebarInterface(9, 5715);
        GetClient()->SendSidebarInterface(10, 2449);
        GetClient()->SendSidebarInterface(11, 4445);
        GetClient()->SendSidebarInterface(12, 147);
        GetClient()->SendSidebarInterface(13, 6299);
        GetClient()->SendSidebarInterface(0, 2423);
        GetClient()->SendPlayerMessage("Welcome to AsioRSPS!");
    }

    void Player::UpdateMapRegion()
    {
        m_CurrentRegion = GetPosition();
        SetUpdateFlag(UpdateFlag::NeedsPlacement);
        GetClient()->SendMapRegion();
    }

}
