#include "epch.h"
#include "LoginHandler.h"
#include "Client.h"
#include "World.h"
#include "UpdateServer.h"
#include "PlayerSave.h"
#include "../vendor/infint/InfInt.h"

namespace Skeleton {

    std::vector<uint8_t> LoginHandler::DecryptRSA(const std::vector<uint8_t>& encrypted)
    {
        InfInt modulus("7358468218802353763968337874150871982730399709440412405836552068652491263995742482684583084259707322427560860706498518284967701262909700914249336447102331");
        InfInt exponent("2568171316487880703163827947502218515662792507347461021387909966374543123447375779543351448391278764529126804817767972559119432449582806569479801590624273");

        InfInt message = 0;
        for (size_t i = 0; i < encrypted.size(); i++) {
            message = message * 256 + encrypted[i];
        }

        InfInt result = 1;
        InfInt base = message % modulus;
        InfInt exp = exponent;

        while (exp > 0) {
            if ((exp % 2).toInt() == 1) {
                result = (result * base) % modulus;
            }
            base = (base * base) % modulus;
            exp = exp / 2;
        }

        InfInt temp = result;
        std::vector<uint8_t> decrypted;

        while (temp > 0) {
            int byte = (temp % 256).toInt();
            decrypted.insert(decrypted.begin(), static_cast<uint8_t>(byte));
            temp = temp / 256;
        }

        return decrypted;
    }

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

        case Client::NetworkStage::UPDATE_SERVER:
            HandleUpdateServerStage(inStream, client);
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

        LOG_INFO("=== NEW CONNECTION: opcode={} ===", request);

        if (request == 15)
        {
            if (inStream.Remaining() < 4)
            {
                return;
            }

            uint32_t clientVersion = inStream.ReadInt();
            if (clientVersion != 464 && clientVersion != 562 && clientVersion != 530 && clientVersion != 602 && clientVersion != 666 && clientVersion != 118785)
            {
                StreamBuffer outStream;
                outStream.WriteByte(6);
                client->Send(outStream);
                client->RequestDisconnect();
                return;
            }

            client->SetNetworkStage(Client::NetworkStage::UPDATE_SERVER);
            StreamBuffer outStream;
            outStream.WriteByte(0);
            client->Send(outStream);
            return;
        }

        if (request != 14)
        {
            LOG_INFO("Invalid login request: {}", request);
            client->Disconnect();
            return;
        }

        inStream.ReadByte();

        StreamBuffer outStream;
        outStream.WriteByte(0);

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

        uint8_t loginSize = inStream.ReadByte();
        LOG_INFO("Login packet size: {}, available: {}", loginSize, inStream.Remaining());
        if (inStream.Remaining() < loginSize)
        {
            LOG_INFO("Waiting for more login data...");
            return;
        }

        uint32_t clientVersion = inStream.ReadInt();
        if (clientVersion != 464 && clientVersion != 562 && clientVersion != 530 && clientVersion != 602 && clientVersion != 666 && clientVersion != 118785)
        {
            LOG_INFO("Invalid client version: {}", clientVersion);
            client->Disconnect();
            return;
        }

        inStream.ReadByte();

        for (int i = 0; i < 16; i++)
        {
            inStream.ReadInt();
        }

        int loginEncryptSize = loginSize - 69;
        loginEncryptSize--;

        uint8_t reportedSize = inStream.ReadByte();
        LOG_INFO("RSA encrypt size: {}, reported size: {}", loginEncryptSize, reportedSize);

        if (reportedSize != loginEncryptSize) {
            LOG_INFO("RSA size mismatch!");
            client->Disconnect();
            return;
        }

        std::vector<uint8_t> rsaPayload(loginEncryptSize);
        for (int i = 0; i < loginEncryptSize; i++) {
            rsaPayload[i] = inStream.ReadByte(false);
        }

        LOG_INFO("RSA payload first 10 bytes: {:02x} {:02x} {:02x} {:02x} {:02x} {:02x} {:02x} {:02x} {:02x} {:02x}",
            rsaPayload[0], rsaPayload[1], rsaPayload[2], rsaPayload[3], rsaPayload[4],
            rsaPayload[5], rsaPayload[6], rsaPayload[7], rsaPayload[8], rsaPayload[9]);

        std::vector<uint8_t> decrypted = DecryptRSA(rsaPayload);
        LOG_INFO("Decrypted size: {}", decrypted.size());
        if (decrypted.size() >= 10) {
            LOG_INFO("Decrypted first 10 bytes: {:02x} {:02x} {:02x} {:02x} {:02x} {:02x} {:02x} {:02x} {:02x} {:02x}",
                decrypted[0], decrypted[1], decrypted[2], decrypted[3], decrypted[4],
                decrypted[5], decrypted[6], decrypted[7], decrypted[8], decrypted[9]);
        }

        // Minimum RSA block size: 1 (opcode) + 8 (clientHalf) + 8 (serverHalf) + 4 (uid) + 2 (min username+null) + 1 (password null) = 24 bytes
        if (decrypted.size() < 24) {
            LOG_WARN("RSA decrypted block too small: {} bytes (need at least 24)", decrypted.size());
            client->Disconnect();
            return;
        }

        StreamBuffer rsaBuffer(reinterpret_cast<const int8_t*>(decrypted.data()), decrypted.size());

        uint8_t rsaOpcode = rsaBuffer.ReadByte();
        LOG_INFO("RSA opcode: {} (expected 10)", rsaOpcode);

        if (rsaOpcode != 10) {
            LOG_INFO("Invalid RSA block opcode!");
            client->Disconnect();
            return;
        }

        int64_t clientHalf = rsaBuffer.ReadLong();
        int64_t serverHalf = rsaBuffer.ReadLong();

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

        std::string username;
        std::string password;

        try {
            rsaBuffer.ReadInt();
            username = rsaBuffer.ReadString();
            password = rsaBuffer.ReadString();
        } catch (const std::exception& e) {
            LOG_WARN("Failed to read login credentials from RSA block: {}", e.what());
            client->Disconnect();
            return;
        }

        LOG_INFO("Login request: username={} (len={}) password={}", username, username.length(), password);

        // Validate username length (RS limit is 12 characters)
        if (username.length() > 12 || username.length() == 0) {
            LOG_WARN("Invalid username length: {} (must be 1-12 characters)", username.length());
            StreamBuffer outStream;
            outStream.WriteByte(LoginResponse::INVALID_CREDENTIALS);
            client->Send(outStream);
            client->RequestDisconnect();
            return;
        }

        client->SetUsername(username);
        client->SetPassword(password);
        uint8_t response = LoginResponse::OK;

        if (PlayerSave::PlayerExists(username)) {
            if (!PlayerSave::VerifyPassword(username, password)) {
                LOG_WARN("Invalid password for existing player: {}", username);
                response = LoginResponse::INVALID_CREDENTIALS;
                StreamBuffer outStream;
                outStream.WriteByte(response);
                client->Send(outStream);
                client->RequestDisconnect();
                return;
            }
            LOG_INFO("Password verified for player: {}", username);
        } else {
            LOG_INFO("New player: {} - will create save file on logout", username);
        }

        auto existingPlayer = World::Instance().GetPlayer(username);
        if (existingPlayer)
        {
            auto existingClient = existingPlayer->GetClient();

            if (!existingClient || existingClient == client   )
            {
                response = LoginResponse::RECONNECT;
                LOG_INFO("Player {} attempting seamless reconnection", username);
            }
            else
            {
                response = LoginResponse::ACCOUNT_ONLINE;
            }
        }

        auto player = std::make_shared<Player>(username, client);
        client->SetPlayer(player);

        bool isNewPlayer = !PlayerSave::PlayerExists(username);
        if (!isNewPlayer) {
            std::string savedPassword;
            if (PlayerSave::LoadPlayer(*player, savedPassword)) {
                LOG_INFO("[SAVE] Loaded player data for: {}", username);
            } else {
                LOG_ERROR("[SAVE] Failed to load player data for: {}", username);
            }
        } else {
            player->SetNewPlayer(true);
            player->RandomizeAppearance();
            LOG_INFO("[SAVE] New player - randomized appearance");
        }

        if (response != LoginResponse::OK && response != LoginResponse::RECONNECT)
        {
            StreamBuffer outStream;
            outStream.WriteByte(response);
            client->Send(outStream);
            LOG_WARN("LOGIN RESPONSE NOT OK: {}", response);
            client->RequestDisconnect();
            return;
        }

        // CRITICAL FIX: The order of operations matters!
        // Original working code does:
        // 1. Send login response packet
        // 2. Set network stage to LOGGED_IN
        // 3. Register player (which calls Player::Login() sending game setup packets)
        //
        // The broken refactored code called RegisterPlayer FIRST, which sent all
        // game packets BEFORE the login response - causing the client to disconnect.

        // Build the login response packet
        StreamBuffer outStream;
        outStream.WriteByte(response);
        outStream.WriteByte(0);  // rights
        outStream.WriteByte(0);  // flagged
        outStream.WriteShort(player->GetIndex());
        outStream.WriteByte(1);  // members

        LOG_INFO("Sending login response: code={}, rights={}, flagged={}, index={}, members={}",
            response, 0, 0, player->GetIndex(), 1);
        LOG_INFO("Login response packet size: {} bytes", outStream.Position());

        // Step 1: Send login response FIRST
        client->Send(outStream);
        
        // Step 2: Set network stage to LOGGED_IN
        client->SetNetworkStage(Client::NetworkStage::LOGGED_IN);
        
        // Step 3: NOW register the player (this calls Player::Login() which sends game setup packets)
        World::Instance().RegisterPlayer(player);

        LOG_INFO("Player {} logged in successfully", username);
    }

    void LoginHandler::HandleUpdateServerStage(StreamBuffer& inStream, std::shared_ptr<Client> client)
    {
        if (inStream.Remaining() == 0)
        {
            return;
        }

        //LOG_INFO("=== HandleUpdateServerStage: {} bytes available ===", inStream.Remaining());

        int requestCount = 0;
        while (inStream.Remaining() >= 4)
        {
            uint8_t type = inStream.ReadByte() & 0xFF;
            uint8_t cache = inStream.ReadByte() & 0xFF;
            uint16_t id = inStream.ReadShort(false) & 0xFFFF;

            requestCount++;

            // TODO: understand the purpose of printing these requests. Type 0 was spamming in the terminal, so I specifically disabled its logging for now. 
            //LOG_INFO("Request #{}: type={}, cache={}, id={}", requestCount, type, cache, id);

            switch (type)
            {
                case 0:
                    //LOG_INFO("  -> Type 0: Non-urgent (queuing not implemented)");
                    break;
                
                LOG_INFO("Request #{}: type={}, cache={}, id={}", requestCount, type, cache, id);

                case 1:
                {
                    LOG_INFO("  -> Type 1: Urgent request");
                    StreamBuffer response = UpdateServer::CreateCacheResponse(cache, id);
                    LOG_INFO("  -> Sending {} bytes", response.Position());
                    client->Send(response);
                    break;
                }
                case 2:
                case 3:
                    LOG_INFO("  -> Type {}: Clear/reset request", type);
                    break;
                case 4:
                    LOG_INFO("  -> Type 4: Encryption key packet (ignoring - encryption not supported)");
                    break;
                default:
                    LOG_WARN("  -> Unknown type: {}", type);
                    break;
            }
        }

        if (inStream.Remaining() > 0 && inStream.Remaining() < 4)
        {
            LOG_INFO("Partial data remaining: {} bytes (waiting for more)", inStream.Remaining());
        }

       // LOG_INFO("=== Processed {} requests ===", requestCount);
    }

}
