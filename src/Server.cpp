#include "epch.h"
#include "Server.h"
#include "World.h"
#include "Constants.h"

namespace Skeleton
{

	Server::Server(const std::string& host, uint32_t port) :
		m_Acceptor(m_IOContext, asio::ip::tcp::endpoint(asio::ip::make_address(host), port))
	{
		Log::Init();
		AcceptClients();
		LOG_INFO("Server Initialized!");
	}

	Server::~Server()
	{
		LOG_INFO("Server Shutdown!");
	}

	void Server::Process()
	{
		World::Instance().Process();
	}

	void Server::Run()
	{
		using namespace std::chrono;

		m_Running = true;

		m_NetworkThread = std::thread([this]() {
			m_IOContext.run(); // blocks, processes all async events
		});


		while (m_Running)
		{
			auto start = steady_clock::now();

			Process();

			auto elapsed = duration_cast<milliseconds>(steady_clock::now() - start).count();
			if (elapsed < CYCLE_RATE_MS)
			{
				LOG_INFO("Sleeping for {0} ms. Players online: {1}. Active sessions: {2}", (CYCLE_RATE_MS - elapsed), World::Instance().GetPlayers().size(), m_Sessions.size());
				std::this_thread::sleep_for(milliseconds(CYCLE_RATE_MS - elapsed));
			}
			else
			{
				LOG_CRITICAL("Cannot handle load! Processing time: {0} ms.", elapsed);
			}
		}

		m_IOContext.stop();
		if (m_NetworkThread.joinable())
			m_NetworkThread.join();
	}


	void Server::AcceptClients()
	{
		m_Acceptor.async_accept(
			[this](std::error_code ec, asio::ip::tcp::socket socket) {
				if (!ec) {
					auto client = std::make_shared<Client>(std::move(socket)); // do something about this?
					std::weak_ptr<Client> weak_client = client;

					client->m_OnDisconnect = [this, weak_client]() {
						if (auto shared_client = weak_client.lock()) {
							m_Sessions.erase(shared_client.get());
						}
						};
					m_Sessions[client.get()] = weak_client;
					client->Start();
					//LOG_INFO("Accepted new connection!");
				}
				AcceptClients(); // Continue accepting more clients
			});
	}

	void Server::Close()
	{
		LOG_INFO("Application Closed!");
		m_Running = false;
	}

}