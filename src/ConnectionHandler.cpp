#include "epch.h"
#include "ConnectionHandler.h"
#include "GameEngine.h"
#include "Client.h"
#include "SessionOpenedTask.h"
#include "SessionClosedTask.h"
#include <fstream>
#include <sstream>
#include <cstdio>
#include <array>
#include <memory>

namespace Skeleton {

ConnectionHandler::ConnectionHandler(GameEngine& engine)
    : m_Engine(engine)
{
    loadWhitelist();
    
    std::ifstream configFile(".whitelisted-ips");
    if (configFile.is_open()) {
        std::string line;
        while (std::getline(configFile, line)) {
            if (line.find("limit_connections_per_ip=") == 0) {
                std::string value = line.substr(25);
                size_t start = value.find_first_not_of(" \t\r\n");
                size_t end = value.find_last_not_of(" \t\r\n");
                if (start != std::string::npos) {
                    value = value.substr(start, end - start + 1);
                    m_LimitConnectionsPerIP = (value == "true" || value == "1" || value == "yes");
                    LOG_INFO("Connection limiting per IP: {}", m_LimitConnectionsPerIP ? "enabled" : "disabled");
                }
            }
            else if (line.find("max_connections_per_ip=") == 0) {
                std::string value = line.substr(23);
                size_t start = value.find_first_not_of(" \t\r\n");
                size_t end = value.find_last_not_of(" \t\r\n");
                if (start != std::string::npos) {
                    value = value.substr(start, end - start + 1);
                    try {
                        m_MaxConnectionsPerIP = std::stoul(value);
                        LOG_INFO("Max connections per IP set to: {}", m_MaxConnectionsPerIP);
                    } catch (const std::exception& e) {
                        LOG_WARN("Invalid max_connections_per_ip value: {}", value);
                    }
                }
            }
        }
        configFile.close();
    }
}

void ConnectionHandler::start(asio::ip::tcp::acceptor& acceptor) {
    // Store pointer to acceptor for use in acceptClients()
    // The acceptor is owned by Server and lives for the duration of the server
    m_Acceptor = &acceptor;
    m_Running = true;
    
    LOG_INFO("ConnectionHandler::start() - Starting accept loop");
    acceptClients();
}

void ConnectionHandler::stop() {
    m_Running = false;
}

void ConnectionHandler::acceptClients() {
    if (!m_Running || !m_Acceptor) {
        return;
    }
    
    // Use the stored pointer - this avoids capturing by reference which could dangle
    // if the lambda outlives the original reference parameter
    m_Acceptor->async_accept(
        [this](std::error_code ec, asio::ip::tcp::socket socket) {
            if (!m_Running) {
                LOG_INFO("ConnectionHandler: Accept loop stopped");
                return;
            }
            
            if (ec) {
                if (ec != asio::error::operation_aborted) {
                    LOG_ERROR("Accept error: {}", ec.message());
                }
                // Still try to accept more connections unless we're shutting down
                if (m_Running) {
                    acceptClients();
                }
                return;
            }
            
            try {
                std::string clientIP = socket.remote_endpoint().address().to_string();
                LOG_INFO("New connection accepted from {}", clientIP);
                
                if (!isIPWhitelisted(clientIP)) {
                    LOG_WARN("Rejected connection from non-whitelisted IP: {}", clientIP);
                    socket.close();
                    acceptClients();
                    return;
                }
                
                if (m_LimitConnectionsPerIP && !isLocalhost(clientIP)) {
                    size_t currentConnections = getConnectionCountForIP(clientIP);
                    if (currentConnections >= m_MaxConnectionsPerIP) {
                        LOG_WARN("Rejected connection from IP: {} (limit: {}/{} connections)", 
                            clientIP, currentConnections, m_MaxConnectionsPerIP);
                        socket.close();
                        acceptClients();
                        return;
                    }
                }
                
                auto client = std::make_shared<Client>(std::move(socket), m_Engine);

                // Set up disconnect callback - capture by raw pointer since client 
                // is stored in m_Sessions and will outlive this callback
                client->m_OnDisconnect = [this, client_ptr = client.get()]() {
                    m_Sessions.erase(client_ptr);
                };

                m_Sessions[client.get()] = client;

                m_Engine.pushTask(std::make_unique<SessionOpenedTask>(client));
            } catch (const std::exception& e) {
                LOG_ERROR("Failed to create client: {}", e.what());
            }

            // Continue accepting more connections
            acceptClients();
        });
}

void ConnectionHandler::loadWhitelist() {
    m_WhitelistedIPs.insert("127.0.0.1");
    
    std::string publicIP = getPublicIP();
    if (!publicIP.empty()) {
        m_WhitelistedIPs.insert(publicIP);
        LOG_INFO("Added public IP to whitelist: {}", publicIP);
    }
    
    std::ifstream whitelistFile(".whitelisted-ips");
    if (!whitelistFile.is_open()) {
        LOG_WARN(".whitelisted-ips not found, only localhost and public IP will be whitelisted");
        return;
    }
    
    std::string line;
    while (std::getline(whitelistFile, line)) {
        if (line.find_first_not_of(" \t\r\n") == std::string::npos) {
            continue;
        }
        
        size_t start = line.find_first_not_of(" \t\r\n");
        size_t end = line.find_last_not_of(" \t\r\n");
        std::string ip = line.substr(start, end - start + 1);
        
        if (!ip.empty() && ip[0] != '#') {
            m_WhitelistedIPs.insert(ip);
            LOG_INFO("Added IP to whitelist: {}", ip);
        }
    }
    
    whitelistFile.close();
    LOG_INFO("Loaded {} whitelisted IPs", m_WhitelistedIPs.size());
}

bool ConnectionHandler::isIPWhitelisted(const std::string& ip) const {
    return m_WhitelistedIPs.find(ip) != m_WhitelistedIPs.end();
}

size_t ConnectionHandler::getConnectionCountForIP(const std::string& ip) const {
    size_t count = 0;
    for (const auto& session : m_Sessions) {
        try {
            std::string sessionIP = session.second->m_Socket.remote_endpoint().address().to_string();
            if (sessionIP == ip) {
                count++;
            }
        } catch (const std::exception&) {
            continue;
        }
    }
    return count;
}

bool ConnectionHandler::isLocalhost(const std::string& ip) const {
    return ip == "127.0.0.1" || ip == "::1" || ip == "localhost";
}

std::string ConnectionHandler::getPublicIP() const {
    std::array<char, 128> buffer;
    std::string result;
    
#ifdef _WIN32
    std::unique_ptr<FILE, decltype(&_pclose)> pipe(
        _popen("curl -s ifconfig.me", "r"), _pclose);
#else
    std::unique_ptr<FILE, decltype(&pclose)> pipe(
        popen("curl -s ifconfig.me 2>/dev/null || curl -s icanhazip.com 2>/dev/null || echo ''", "r"), pclose);
#endif
    
    if (!pipe) {
        LOG_WARN("Failed to execute command to get public IP");
        return "";
    }
    
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    
    if (result.find_first_not_of(" \t\r\n") == std::string::npos) {
        return "";
    }
    
    size_t start = result.find_first_not_of(" \t\r\n");
    size_t end = result.find_last_not_of(" \t\r\n");
    return result.substr(start, end - start + 1);
}

size_t ConnectionHandler::getUniqueIPCount() const {
    std::unordered_set<std::string> uniqueIPs;
    for (const auto& session : m_Sessions) {
        try {
            std::string sessionIP = session.second->m_Socket.remote_endpoint().address().to_string();
            uniqueIPs.insert(sessionIP);
        } catch (const std::exception&) {
            continue;
        }
    }
    return uniqueIPs.size();
}

}
