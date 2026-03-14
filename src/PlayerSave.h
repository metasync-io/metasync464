#pragma once
#include "epch.h"
#include <vector>
#include <string>
#include <cstdint>

namespace Skeleton {

    class Player;

    class SerializationBuffer {
    public:
        SerializationBuffer();

        void WriteByte(uint8_t value);
        void WriteShort(uint16_t value);
        void WriteInt(uint32_t value);
        void WriteDouble(double value);
        void WriteString(const std::string& str);

        uint8_t ReadByte();
        uint16_t ReadShort();
        uint32_t ReadInt();
        double ReadDouble();
        std::string ReadString();

        const std::vector<uint8_t>& GetData() const { return m_Data; }
        void SetData(const std::vector<uint8_t>& data);
        size_t Size() const { return m_Data.size(); }
        bool HasRemaining() const { return m_Position < m_Data.size(); }
        void Clear();

    private:
        std::vector<uint8_t> m_Data;
        size_t m_Position;
    };

    class PlayerSave {
    public:

        static bool SavePlayer(const Player& player, const std::string& password);

        static bool LoadPlayer(Player& player, std::string& password);

        static bool PlayerExists(const std::string& username);

        static bool VerifyPassword(const std::string& username, const std::string& password);

    private:

        static void SerializePlayer(SerializationBuffer& buffer, const Player& player, const std::string& password);
        static void DeserializePlayer(SerializationBuffer& buffer, Player& player, std::string& password);

        static bool WriteCompressedFile(const std::string& filename, const std::vector<uint8_t>& data);
        static bool ReadCompressedFile(const std::string& filename, std::vector<uint8_t>& data);

        static std::string FormatUsername(const std::string& username);
        static std::string GetPlayerFilePath(const std::string& username);

        static constexpr const char* SAVE_DIRECTORY = "data/savedGames/";
    };

}
