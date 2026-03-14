#pragma once
#include "Client.h"
#include "GameEngine.h"
#include "ConnectionHandler.h"

namespace Skeleton
{

    class Server
    {
    public:

        Server(const std::string& host, uint32_t port);

        ~Server();

        void bind(const std::string& host, uint32_t port);

        void start();

        void Close();

    private:

        GameEngine m_Engine;

        // IMPORTANT: m_IOContext MUST be declared before m_Acceptor and m_IOWorkGuard
        // because they depend on it being constructed first (member initialization order)
        asio::io_context m_IOContext;
        
        // Work guard keeps io_context.run() alive even when there's no pending work
        // Uses executor_work_guard (modern ASIO) instead of deprecated io_context::work
        asio::executor_work_guard<asio::io_context::executor_type> m_IOWorkGuard;
        
        asio::ip::tcp::acceptor m_Acceptor;

        std::unique_ptr<ConnectionHandler> m_ConnectionHandler;

        std::thread m_NetworkThread;
    };
}
