#pragma once
#include "epch.h"
#include "ISAACCipher.h"
#include "LoginHandler.h"
#include "StreamBuffer.h"

namespace Skeleton
{
	class Player;

	class Client : public std::enable_shared_from_this<Client>
	{
	public:
		enum class NetworkStage {
			CONNECTED, LOGGING_IN, LOGGED_IN, LOGGED_OUT
		};
	public:
		explicit Client(asio::ip::tcp::socket socket) : m_Socket(std::move(socket)), m_Strand(m_Socket.get_executor()) {}
		~Client();
		void Start();
		void Disconnect();
		void RequestDisconnect();

		void Send(StreamBuffer& outStream);

		void SendMapRegion();
		void SendPlayerMessage(std::string message);
		void SendSidebarInterface(int32_t menuId, int32_t form);
	private:
		void HandleIncomingData(StreamBuffer& inStream);
		void HandlePacket(StreamBuffer& inStream, int32_t packetOpcode, int32_t packetLength);
		void ReadIncomingData();
		void DoWrite();
	public:
		NetworkStage GetNetworkStage() const { return m_Stage; }
		void SetNetworkStage(NetworkStage stage) { m_Stage = stage; }

		void SetUsername(const std::string& name) { m_Username = name; }
		void SetPlayer(std::weak_ptr<Player> player) { m_Player = player; }
		std::shared_ptr<Player> GetPlayer() const { return m_Player.lock(); }

		const std::string& GetUsername() const { return m_Username; }
		std::function<void()> m_OnDisconnect;

		void SetDecryptor(std::shared_ptr<ISAACCipher> decryptor) { m_Decryptor = decryptor; }
		void SetEncryptor(std::shared_ptr<ISAACCipher> encryptor) { m_Encryptor = encryptor; }

		std::shared_ptr<ISAACCipher> GetEncryptor() { return m_Encryptor; }
	private:
		asio::ip::tcp::socket m_Socket;
		asio::strand<decltype(m_Socket.get_executor())> m_Strand;
		NetworkStage m_Stage = NetworkStage::CONNECTED;

		LoginHandler m_LoginHandler;
		std::string m_Username = "";
		std::atomic_bool m_Disconnected = false;

		std::shared_ptr<ISAACCipher> m_Encryptor = nullptr;
		std::shared_ptr<ISAACCipher> m_Decryptor = nullptr;

		uint32_t m_PacketsThisSecond = 0;
		std::chrono::steady_clock::time_point m_LastPacketSecond;
		std::chrono::steady_clock::time_point m_LastActivityTime;

		int8_t m_asioData[512]{};
		std::deque<std::vector<uint8_t>> m_WriteQueue;
		bool m_Writing = false;

		std::atomic<bool> m_PendingDisconnect = false;

		std::weak_ptr<Player> m_Player;
	};
}