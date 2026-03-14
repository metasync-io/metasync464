#pragma once
#include "epch.h"

namespace Skeleton {

class GameEngine;

class ConnectionHandler {
public:
    explicit ConnectionHandler(GameEngine& engine);

    void start(asio::ip::tcp::acceptor& acceptor);
    
    void stop();
    
    size_t getTotalConnections() const { return m_Sessions.size(); }
    size_t getUniqueIPCount() const;

private:
    void acceptClients();
    
    void loadWhitelist();
    bool isIPWhitelisted(const std::string& ip) const;
    std::string getPublicIP() const;
    size_t getConnectionCountForIP(const std::string& ip) const;
    bool isLocalhost(const std::string& ip) const;

private:
    GameEngine& m_Engine;
    
    // Store pointer to acceptor to avoid dangling reference in lambda
    asio::ip::tcp::acceptor* m_Acceptor = nullptr;
    
    std::unordered_map<void*, std::shared_ptr<class Client>> m_Sessions;
    
    std::atomic<bool> m_Running{false};
    
    std::unordered_set<std::string> m_WhitelistedIPs;
    bool m_LimitConnectionsPerIP{true};
    size_t m_MaxConnectionsPerIP{3};
};

}
