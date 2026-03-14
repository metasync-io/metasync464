#include "epch.h"
#include "Client.h"
#include "PacketOpcodes.h"
#include "Constants.h"
#include "World.h"
#include "Packet.h"
#include "PlayerSave.h"
#include "GameEngine.h"
#include "TextUtils.h"
#include "SessionMessageTask.h"
#include "packet/incoming/PacketManager.h"
#include "packet/incoming/impl/DefaultPacketHandler.h"

namespace Skeleton {

    Client::~Client()
    {
        //LOG_INFO("Destroyed client!");
    }

    void Client::Start()
	{
        ReadIncomingData();
		//LOG_INFO("Started Client!");
	}

    void Client::HandleIncomingData(StreamBuffer& inStream)
    {
        using namespace std::chrono;

        auto now = steady_clock::now();
        if (m_LastPacketSecond.time_since_epoch().count() == 0)
        {

            m_LastPacketSecond = now;
            m_LastActivityTime = now;
        }

        if (duration_cast<seconds>(now - m_LastPacketSecond).count() >= 1)
        {
            m_PacketsThisSecond = 0;
            m_LastPacketSecond = now;
        }

        m_PacketsThisSecond++;
        m_LastActivityTime = now;

        if (m_PacketsThisSecond > MAX_PACKETS_PER_SECOND)
        {
            LOG_WARN("Client flood detected ({} packets/sec). Disconnecting.", m_PacketsThisSecond);
            Disconnect();
            return;
        }

        int32_t packetOpcode = -1;
        int32_t packetLength = -1;

        while (inStream.HasRemaining())
        {

            if (GetNetworkStage() != NetworkStage::LOGGED_IN)
            {
                m_LoginHandler.Process(inStream, shared_from_this());
                break;
            }

            if (packetOpcode == -1)
            {
                int32_t rawByte = inStream.ReadByte() & 0xff;
                int32_t key = m_Decryptor->GetNextKey();
                packetOpcode = (rawByte - key) & 0xFF;
                // Log ALL packets to debug the missing STOP
                LOG_INFO("[PACKET-RAW] raw={} key={} -> opcode={}", rawByte, key, packetOpcode);
            }

            if (packetLength == -1)
            {
                packetLength = PacketLengths[packetOpcode];

                if (packetLength == -1)
                {
                    if (!inStream.HasRemaining())
                        break;
                    packetLength = inStream.ReadByte() & 0xff;
                }
                else if (packetLength == -3)
                {
                    if (!inStream.HasRemaining())
                        break;
                    packetLength = inStream.Remaining();
                }

            }

            if (inStream.Remaining() >= packetLength)
            {
                HandlePacket(inStream, packetOpcode, packetLength);

                packetOpcode = -1;
                packetLength = -1;
            }

        }

        inStream.Clear();
    }

    void Client::HandlePacket(StreamBuffer& inStream, int32_t packetOpcode, int32_t packetLength)
    {
        int32_t positionBefore = inStream.Position();

        // Opcode 201 (MoveControl) now handled by standard PacketManager
        // No fast queue - goes through normal 600ms tick

        std::string clientIP;
        bool isLocalhost = false;
        try {
            clientIP = m_Socket.remote_endpoint().address().to_string();
            if (clientIP == "127.0.0.1" || clientIP == "::1") {
                clientIP = "localhost";
                isLocalhost = true;
            }
        } catch (const std::exception&) {
            clientIP = "disconnected";
        }

        auto handler = PacketManager::getInstance().getHandler(packetOpcode);
        if (handler && GetPlayer()) {
            // Suppress logging for opcode 202
            if (packetOpcode != 202) {
                std::string packetName = GetIncomingPacketName(packetOpcode);
                if (packetName == "UNKNOWN") {
                    packetName = "\033[93mUNKNOWN\033[0m";
                }
                if (isLocalhost) {
                    LOG_INFO("\033[94m[RECV]\033[0m {} ({}) - {} bytes [\033[92m{}\033[0m]", packetName, packetOpcode, packetLength, clientIP);
                } else {
                    LOG_INFO("\033[94m[RECV]\033[0m {} ({}) - {} bytes [\033[38;5;208m{}\033[0m]", packetName, packetOpcode, packetLength, clientIP);
                }
            }
            handler->handle(GetPlayer(), inStream, packetOpcode, packetLength);
        } else {
            // Suppress logging for opcode 202 even when no handler
            if (packetOpcode != 202) {
                std::string hexDump;
                if (packetLength > 0 && packetLength < 100) {
                    int bytesToRead = std::min(packetLength, 50);
                    int startPos = inStream.Position();
                    for (int i = 0; i < bytesToRead && inStream.HasRemaining(); i++) {
                        char buf[4];
                        uint8_t byte = inStream.Data()[startPos + i];
                        snprintf(buf, sizeof(buf), "%02x ", byte);
                        hexDump += buf;
                    }
                    if (!hexDump.empty() && hexDump.back() == ' ') {
                        hexDump.pop_back();
                    }
                }
                
                std::string hexPart = hexDump.empty() ? "" : " (Hex: " + hexDump + ")";
                
                if (isLocalhost) {
                    LOG_WARN("\033[94m[RECV]\033[0m \033[93mUNKNOWN\033[0m ({}) - {} bytes{} [\033[92m{}\033[0m]", packetOpcode, packetLength, hexPart, clientIP);
                } else {
                    LOG_WARN("\033[94m[RECV]\033[0m \033[93mUNKNOWN\033[0m ({}) - {} bytes{} [\033[38;5;208m{}\033[0m]", packetOpcode, packetLength, hexPart, clientIP);
                }
            }
        }

        int read = inStream.Position() - positionBefore;
        for (int i = read; i < packetLength; i++) {
            inStream.ReadByte();
        }
    }

    void Client::HandleIncomingDataTask(StreamBuffer& inStream) {
        HandleIncomingData(inStream);
    }

    void Client::ReadIncomingData()
	{
        auto self = shared_from_this();

        m_Socket.async_read_some(
            asio::buffer(m_asioData, sizeof(m_asioData)),
            asio::bind_executor(m_Strand, [this, self](std::error_code ec, std::size_t length) {
                if (!ec)
                {
                    std::vector<uint8_t> dataCopy(m_asioData, m_asioData + length);
                    StreamBuffer inStream(reinterpret_cast<const int8_t*>(dataCopy.data()), length);

                    m_Engine.pushTask(std::make_unique<SessionMessageTask>(self, std::move(inStream)));

                    ReadIncomingData();
                }
                else
                {
                   // LOG_WARN("Disconnecting because cannot read: {}", ec.message());
                    Disconnect();
                }
            }));
	}

    void Client::Disconnect()
    {

        if (m_Disconnected.exchange(true))
        {
            return;
        }

        if (m_Socket.is_open())
        {
            std::error_code ec;
            m_Socket.shutdown(asio::ip::tcp::socket::shutdown_both, ec);
            m_Socket.close(ec);
        }

        if (m_Player.lock())
        {
            auto player = m_Player.lock();
            if (player && !m_Username.empty() && !m_Password.empty()) {
                LOG_INFO("[SAVE] Saving player data for: {}", m_Username);
                if (PlayerSave::SavePlayer(*player, m_Password)) {
                    LOG_INFO("[SAVE] Successfully saved player: {}", m_Username);
                } else {
                    LOG_ERROR("[SAVE] Failed to save player: {}", m_Username);
                }
            }

            World::Instance().RemovePlayer(m_Username);
        }

        SetNetworkStage(NetworkStage::LOGGED_OUT);

        if (m_OnDisconnect)
        {
            m_OnDisconnect();
        }

      //  LOG_INFO("Client fully disconnected and cleaned up.");
    }

    void Client::RequestDisconnect()
    {
        asio::post(m_Strand, [this]() {
            m_PendingDisconnect = true;

            if (!m_Writing && m_WriteQueue.empty()) {
                Disconnect();
            }

        });
    }

    void Client::Send(StreamBuffer& outStream)
    {
        uint8_t opcode = outStream.Data()[0];
        bool shouldSuppressUpdateLogs = (opcode == 90 || opcode == 69 || opcode == 202);
        std::string clientIP;
        bool isLocalhost = false;
        try {
            clientIP = m_Socket.remote_endpoint().address().to_string();
            if (clientIP == "127.0.0.1" || clientIP == "::1") {
                clientIP = "localhost";
                isLocalhost = true;
            }
        } catch (const std::exception&) {
            clientIP = "disconnected";
        }
        
        std::string extraInfo = "";
        if (opcode == 221) {
            auto player = m_Player.lock();
            if (player) {
                int32_t regionX = player->GetPosition().GetRegionX();
                int32_t regionY = player->GetPosition().GetRegionY();
                int32_t posX = player->GetPosition().GetX();
                int32_t posY = player->GetPosition().GetY();
                extraInfo = " | " + player->GetUsername() + " - Region (" + std::to_string(regionX) + "," + std::to_string(regionY) + ") at pos (" + std::to_string(posX) + "," + std::to_string(posY) + ")";
            }
        }
        
        if (!shouldSuppressUpdateLogs) {
            if (isLocalhost) {
                LOG_INFO("\033[91m[SEND]\033[0m {} ({}) - {} bytes [\033[92m{}\033[0m]{}", GetOutgoingPacketName(opcode), opcode, outStream.Position(), clientIP, extraInfo);
            } else {
                LOG_INFO("\033[91m[SEND]\033[0m {} ({}) - {} bytes [\033[38;5;208m{}\033[0m]{}", GetOutgoingPacketName(opcode), opcode, outStream.Position(), clientIP, extraInfo);
            }
        }

        auto self = shared_from_this();

        asio::post(m_Strand, [this, self, data = std::vector<uint8_t>(outStream.Data().begin(),
                                                                        outStream.Data().begin() + outStream.Position())]() mutable {
            m_WriteQueue.push_back(std::move(data));

            if (!m_Writing)
                DoWrite();
        });
    }


    void Client::DoWrite()
    {
        m_Writing = true;
        auto& front = m_WriteQueue.front();
        auto self = shared_from_this();

        asio::async_write(m_Socket, asio::buffer(front),
            asio::bind_executor(m_Strand, [this, self](std::error_code ec, std::size_t  ) {
                if (!ec) {
                    m_WriteQueue.pop_front();

                    if (!m_WriteQueue.empty()) {
                        DoWrite();
                    }
                    else {
                        m_Writing = false;

                        if (m_PendingDisconnect) {
                            Disconnect();
                            m_PendingDisconnect = false;
                        }
                    }
                }
                else {
                    LOG_ERROR("Write error: {}", ec.message());
                    Disconnect();
                }
            }));
    }


}
