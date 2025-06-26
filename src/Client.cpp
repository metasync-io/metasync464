#include "epch.h"
#include "Client.h"
#include "Constants.h"
#include "World.h"
#include "Packet.h"

namespace Skeleton {

    Client::~Client()
    {
        LOG_INFO("Destroyed client!");
    }

    void Client::Start()
	{
        ReadIncomingData();

		LOG_INFO("Started Client!");
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

        // Rate limiting
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

        // Append new data to the receive buffer
        
        int32_t packetOpcode = -1;
        int32_t packetLength = -1;
        while (inStream.HasRemaining())
        {
            if (GetNetworkStage() != NetworkStage::LOGGED_IN)
            {
                m_LoginHandler.Process(inStream, shared_from_this());
                break;
            }

            // Decode the packet opcode.
            if (packetOpcode == -1)
            {
                packetOpcode = inStream.ReadByte() & 0xff;
                packetOpcode = (packetOpcode - m_Decryptor->GetNextKey()) & 0xFF;
            }

            // Decode the packet length.
            if (packetLength == -1)
            {
                packetLength = PacketLengths[packetOpcode];
                if (packetLength == -1) // check for variable packet lengths
                {
                    if (!inStream.HasRemaining())
                        break;
                    packetLength = inStream.ReadByte() & 0xff;
                }
            }

            if (inStream.Remaining() >= packetLength)
            {
                // Decode the packet payload.
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
        switch (packetOpcode)
        {

            case 248: // Movement.
            case 164: // ^
            case 98:  // ^
            {
                int32_t length = packetLength;
                if (packetOpcode == 248) {
                    length -= 14;
                }

                int32_t steps = (length - 5) / 2;
                std::vector<std::array<int32_t, 2>> path(steps);

                int32_t firstStepX = inStream.ReadShort(ValueType::A, ByteOrder::LITTLE);

                for (int i = 0; i < steps; ++i) {
                    path[i][0] = inStream.ReadByte();
                    path[i][1] = inStream.ReadByte();
                }

                int32_t firstStepY = inStream.ReadShort(ByteOrder::LITTLE);

                GetPlayer()->GetMovementHandler().Reset();
                GetPlayer()->GetMovementHandler().SetRunPath(inStream.ReadByte(ValueType::C) == 1);
                GetPlayer()->GetMovementHandler().AddToPath(Position(firstStepX, firstStepY));
                for (int i = 0; i < steps; ++i) 
                {
                    path[i][0] += firstStepX;
                    path[i][1] += firstStepY;
                    GetPlayer()->GetMovementHandler().AddToPath(Position(path[i][0], path[i][1]));
                }

                GetPlayer()->GetMovementHandler().Finish();
                break;
            }
            default:
                break;
        }
        int read = inStream.Position() - positionBefore;
        for (int i = read; i < packetLength; i++) {
            inStream.ReadByte();
        }
        //LOG_INFO("HANDLING PACKET: {}, LENGTH: {}", packetOpcode, packetLength);
    }

    void Client::ReadIncomingData()
	{
        auto self = shared_from_this();
        m_Socket.async_read_some(
            asio::buffer(m_asioData, sizeof(m_asioData)),
            asio::bind_executor(m_Strand, [this, self](std::error_code ec, std::size_t length) {
                if (!ec) 
                {
                    StreamBuffer inStream(m_asioData, length);
                    HandleIncomingData(inStream);
                    ReadIncomingData();
                }
                else
                {
                    //LOG_WARN("Disconnecting because cannot read");
                    Disconnect();
                }
            }));
	}

    void Client::Disconnect()
    {
        if (m_Disconnected.exchange(true))
        {
            return; // Already disconnected, do nothing
        }

        if (m_Socket.is_open()) 
        {
            std::error_code ec;
            m_Socket.shutdown(asio::ip::tcp::socket::shutdown_both, ec);
            m_Socket.close(ec);
        }

        if (m_Player.lock())
        {
            World::Instance().RemovePlayer(m_Username);
        }

        SetNetworkStage(NetworkStage::LOGGED_OUT);

        if (m_OnDisconnect)
        {
            m_OnDisconnect();
        }

        LOG_INFO("Client fully disconnected and cleaned up.");
    }

    void Client::RequestDisconnect()
    {
        asio::post(m_Strand, [this]() {
            m_PendingDisconnect = true;
            // If nothing is currently writing, disconnect immediately
            if (!m_Writing && m_WriteQueue.empty()) {
                Disconnect();
            }
        });
    }

    void Client::Send(StreamBuffer& outStream)
    {
        auto self = shared_from_this();
        asio::post(m_Strand, [this, self, data = std::vector<uint8_t>(outStream.Data().begin(), outStream.Data().begin() + outStream.Position())]() mutable {
            m_WriteQueue.push_back(std::move(data));

            if (!m_Writing)
                DoWrite();
        });
    }

    void Client::SendMapRegion()
    {
        StreamBuffer outStream(5);
        outStream.WriteHeader(73, *m_Encryptor.get());
        outStream.WriteShort(m_Player.lock()->GetPosition().GetRegionX() + 6, ValueType::A);
        outStream.WriteShort(m_Player.lock()->GetPosition().GetRegionY() + 6);
        Send(outStream);
    }

    void Client::SendPlayerMessage(std::string message)
    {
        StreamBuffer outStream(message.size() + 3);
        outStream.WriteHeader(253, *m_Encryptor.get(), VariableHeaderSize::BYTE);
        outStream.WriteString(message);
        outStream.FinishVariableHeader();
        Send(outStream);
    }

    void Client::SendSidebarInterface(int32_t menuId, int32_t form)
    {
        StreamBuffer outStream(4);
        outStream.WriteHeader(71, *m_Encryptor.get());
        outStream.WriteShort(form);
        outStream.WriteByte(menuId, ValueType::A);
        Send(outStream);
    }
    
    void Client::DoWrite()
    {
        m_Writing = true;
        auto& front = m_WriteQueue.front();
        auto self = shared_from_this();

        asio::async_write(m_Socket, asio::buffer(front),
            asio::bind_executor(m_Strand, [this, self](std::error_code ec, std::size_t /*length*/) {
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