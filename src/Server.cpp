#include "epch.h"
#include "Server.h"
#include "World.h"
#include "Constants.h"
#include "UpdateServer.h"

namespace Skeleton
{

Server::Server(const std::string& host, uint32_t port) 
    : m_IOWorkGuard(asio::make_work_guard(m_IOContext)),  // Modern work guard
      m_Acceptor(m_IOContext)
{
    std::cout << "Server::Server() - Constructor body entered" << std::endl;
    std::cout << "Server::Server() - Initializing log system" << std::endl;
    Log::Init();

    LOG_INFO("Server::Server() - Initializing UpdateServer");
    if (!UpdateServer::Initialize())
    {
        LOG_ERROR("Failed to initialize UpdateServer - cache files may not be available");
    }

    LOG_INFO("Server::Server() - Initializing World with GameEngine");
    World::Instance().init(m_Engine);

    LOG_INFO("Server::Server() - Creating ConnectionHandler");
    m_ConnectionHandler = std::make_unique<ConnectionHandler>(m_Engine);

    LOG_INFO("Server Initialized!");
}

Server::~Server()
{
    UpdateServer::Shutdown();
    LOG_INFO("Server Shutdown!");
}

void Server::bind(const std::string& host, uint32_t port)
{
    LOG_INFO("Binding to {}:{}...", host, port);
    m_Acceptor.open(asio::ip::tcp::v4());
    m_Acceptor.set_option(asio::socket_base::reuse_address(true));
    m_Acceptor.bind(asio::ip::tcp::endpoint(asio::ip::make_address(host), port));
    m_Acceptor.listen();
}

void Server::start()
{
    LOG_INFO("Server::start() - Starting server...");

    // CRITICAL FIX: Register async_accept BEFORE starting the network thread
    // This ensures io_context has work to do when run() is called
    LOG_INFO("Server::start() - Starting connection handler (registering async_accept)");
    m_ConnectionHandler->start(m_Acceptor);

    // Now start the network thread - io_context already has async_accept pending
    LOG_INFO("Server::start() - Starting network thread");
    m_NetworkThread = std::thread([this]() {
        LOG_INFO("Network thread started");
        m_IOContext.run();
        LOG_INFO("Network thread finished");
    });

    LOG_INFO("Server::start() - Starting GameEngine");
    m_Engine.start();

    LOG_INFO("Server ready!");
}

void Server::Close()
{
    LOG_INFO("Shutting down server...");

    // CRITICAL FIX: Stop GameEngine FIRST to ensure threads stop accessing World
    // This prevents race conditions where threads try to access destroyed objects
    LOG_INFO("Stopping GameEngine...");
    m_Engine.stop();

    // Now it's safe to stop the network layer
    LOG_INFO("Stopping network...");
    m_IOWorkGuard.reset();  // Allow io_context to exit when work completes
    m_IOContext.stop();     // Force stop any pending operations

    if (m_NetworkThread.joinable()) {
        m_NetworkThread.join();
    }

    LOG_INFO("Server closed!");
}

}
