#pragma once
#include "epch.h"
#include "StreamBuffer.h"
#include <vector>

namespace Skeleton {

    class Client;

    class LoginHandler
    {
    public:
        void Process(StreamBuffer& inStream, std::shared_ptr<Client> client);

    private:
        void HandleConnectedStage(StreamBuffer& inStream, std::shared_ptr<Client> client);
        void HandleLoggingInStage(StreamBuffer& inStream, std::shared_ptr<Client> client);
        void HandleUpdateServerStage(StreamBuffer& inStream, std::shared_ptr<Client> client);
        std::vector<uint8_t> DecryptRSA(const std::vector<uint8_t>& encrypted);
    };

    namespace LoginResponse
    {
        inline constexpr uint8_t OK = 2;
        inline constexpr uint8_t INVALID_CREDENTIALS = 3;
        inline constexpr uint8_t ACCOUNT_ONLINE = 5;
        inline constexpr uint8_t RECONNECT = 15;
    }

}
