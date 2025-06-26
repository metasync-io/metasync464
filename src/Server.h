#pragma once
#include "Client.h"

namespace Skeleton
{
	class Server
	{
	public:
		Server(const std::string& host, uint32_t port);
		~Server();
		void Run();
		void Close();
	private:
		void Process();
		void AcceptClients();
	private:
		bool m_Running = false;
		asio::io_context m_IOContext;
		asio::ip::tcp::acceptor m_Acceptor;
		std::unordered_map<void*, std::weak_ptr<Client>> m_Sessions;
		std::thread m_NetworkThread;
	};
}