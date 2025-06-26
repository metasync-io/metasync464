#include "epch.h"
#include "LoginHandler.h"
#include "Client.h"
#include "World.h"

namespace Skeleton {

    void LoginHandler::Process(StreamBuffer& inStream, std::shared_ptr<Client> client)
    {
        switch (client->GetNetworkStage())
        {
        case Client::NetworkStage::CONNECTED:
            HandleConnectedStage(inStream, client);
            break;

        case Client::NetworkStage::LOGGING_IN:
            HandleLoggingInStage(inStream, client);
            break;
        default:
            LOG_WARN("You forgot to implement something");
        }
    }

    void LoginHandler::HandleConnectedStage(StreamBuffer& inStream, std::shared_ptr<Client> client)
    {
        if (inStream.Remaining() < 2)
        {
            client->Disconnect();
            return;
        }

        uint8_t request = inStream.ReadByte();
        inStream.ReadByte(); // Name hash

        if (request != 14) 
        {
            LOG_INFO("Invalid login request: {}", request);
            client->Disconnect();
            return;
        }
        StreamBuffer outStream;
        outStream.WriteLong(0); // Padding
        outStream.WriteByte(0); // Opcode

        std::random_device rd;
        std::mt19937_64 gen(rd());
        outStream.WriteLong(static_cast<int64_t>(gen()));

        client->Send(outStream);
        client->SetNetworkStage(Client::NetworkStage::LOGGING_IN);
    }

    void LoginHandler::HandleLoggingInStage(StreamBuffer& inStream, std::shared_ptr<Client> client)
    {

        if (inStream.Remaining() < 2)
        {
            client->Disconnect();
            return;
        }

        uint8_t loginType = inStream.ReadByte();
        if (loginType != 16 && loginType != 18)
        {
            LOG_INFO("Invalid login type: {}", loginType);
            client->Disconnect();
            return;
        }

        uint8_t blockLength = inStream.ReadByte();
        if (inStream.Remaining() < blockLength)
        {
            client->Disconnect();
            return;
        }

        uint8_t magic = inStream.ReadByte(); // Should be 255

        uint16_t clientVersion = inStream.ReadShort();
        if (clientVersion != 317)
        {
            LOG_INFO("Invalid client version: {}", clientVersion);
            client->Disconnect();
            return;
        }

        inStream.ReadByte(); // Skip high/low memory

        for (int i = 0; i < 9; i++) 
        {
            inStream.ReadInt();
        }

        inStream.ReadByte(); // Skip RSA block length

        uint8_t rsaOpcode = inStream.ReadByte();
        if (rsaOpcode != 10) {
            LOG_INFO("Unable to decode RSA block properly!");
            client->Disconnect();
            return;
        }

        int64_t clientHalf = inStream.ReadLong();
        int64_t serverHalf = inStream.ReadLong();

        uint32_t seed[4] =
        {
            static_cast<uint32_t>(clientHalf >> 32),
            static_cast<uint32_t>(clientHalf),
            static_cast<uint32_t>(serverHalf >> 32),
            static_cast<uint32_t>(serverHalf)
        };

        uint32_t inSeed[4] = { seed[0], seed[1], seed[2], seed[3] };
        uint32_t outSeed[4] = { seed[0] + 50, seed[1] + 50, seed[2] + 50, seed[3] + 50 };

        client->SetDecryptor(std::make_unique<ISAACCipher>(inSeed));
        client->SetEncryptor(std::make_unique<ISAACCipher>(outSeed));

        inStream.ReadInt(); // Skip user ID
        std::string username = inStream.ReadString(); // TODO: need to sanitize
        std::string password = inStream.ReadString(); // TODO: need to sanitize

        // TODO: check credentials
        client->SetUsername(username);
        uint8_t response = LoginResponse::OK;

        auto existingPlayer = World::Instance().GetPlayer(username);
        if (existingPlayer)
        {
            auto existingClient = existingPlayer->GetClient();
            // we should now reconnect
            if (!existingClient || existingClient == client /* || loginType == 18 */ ) // todo: check for grace period, we also could check for reconnections from same IP/session with logintype 18 for more seamless reconnections
            {
                response = LoginResponse::RECONNECT;
                LOG_INFO("Player {} attempting seamless reconnection", username);
            }
            else
            {
                response = LoginResponse::ACCOUNT_ONLINE;
            }
        }

        StreamBuffer outStream;
        outStream.WriteByte(response);
        if (response == LoginResponse::OK)
        {
            outStream.WriteByte(0); // staff rights, if any
            outStream.WriteByte(0); // future usage
        }

        client->Send(outStream);

        if (response != LoginResponse::OK && response != LoginResponse::RECONNECT)
        {
            LOG_WARN("LOGIN RESPONSE NOT OK: {}", response);
            client->RequestDisconnect(); // kill socket or something else...?
            return;
        }

        // TODO: check if player existing the world and we want to take control of that player
        auto player = std::make_shared<Player>(username, client);
        client->SetPlayer(player);
        client->SetNetworkStage(Client::NetworkStage::LOGGED_IN);
        World::Instance().RegisterPlayer(player); // this crashes
    }

}