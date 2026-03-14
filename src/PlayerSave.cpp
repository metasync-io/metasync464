#include "epch.h"
#include "PlayerSave.h"
#include "Player.h"
#include <fstream>
#include <filesystem>
#include <zlib.h>
#include <cstring>

namespace Skeleton {

    SerializationBuffer::SerializationBuffer() : m_Position(0) {
        m_Data.reserve(1024);
    }

    void SerializationBuffer::WriteByte(uint8_t value) {
        m_Data.push_back(value);
    }

    void SerializationBuffer::WriteShort(uint16_t value) {

        m_Data.push_back((value >> 8) & 0xFF);
        m_Data.push_back(value & 0xFF);
    }

    void SerializationBuffer::WriteInt(uint32_t value) {

        m_Data.push_back((value >> 24) & 0xFF);
        m_Data.push_back((value >> 16) & 0xFF);
        m_Data.push_back((value >> 8) & 0xFF);
        m_Data.push_back(value & 0xFF);
    }

    void SerializationBuffer::WriteDouble(double value) {

        uint64_t bits;
        std::memcpy(&bits, &value, sizeof(double));

        for (int i = 7; i >= 0; i--) {
            m_Data.push_back((bits >> (i * 8)) & 0xFF);
        }
    }

    void SerializationBuffer::WriteString(const std::string& str) {

        for (char c : str) {
            m_Data.push_back(static_cast<uint8_t>(c));
        }
        m_Data.push_back(0);
    }

    uint8_t SerializationBuffer::ReadByte() {
        if (m_Position >= m_Data.size()) {
            throw std::runtime_error("SerializationBuffer: Read past end of buffer");
        }
        return m_Data[m_Position++];
    }

    uint16_t SerializationBuffer::ReadShort() {
        uint16_t value = 0;
        value |= (static_cast<uint16_t>(ReadByte()) << 8);
        value |= static_cast<uint16_t>(ReadByte());
        return value;
    }

    uint32_t SerializationBuffer::ReadInt() {
        uint32_t value = 0;
        value |= (static_cast<uint32_t>(ReadByte()) << 24);
        value |= (static_cast<uint32_t>(ReadByte()) << 16);
        value |= (static_cast<uint32_t>(ReadByte()) << 8);
        value |= static_cast<uint32_t>(ReadByte());
        return value;
    }

    double SerializationBuffer::ReadDouble() {
        uint64_t bits = 0;
        for (int i = 7; i >= 0; i--) {
            bits |= (static_cast<uint64_t>(ReadByte()) << (i * 8));
        }

        double value;
        std::memcpy(&value, &bits, sizeof(double));
        return value;
    }

    std::string SerializationBuffer::ReadString() {
        std::string result;
        uint8_t byte;

        while (HasRemaining() && (byte = ReadByte()) != 0) {
            result += static_cast<char>(byte);
        }

        return result;
    }

    void SerializationBuffer::SetData(const std::vector<uint8_t>& data) {
        m_Data = data;
        m_Position = 0;
    }

    void SerializationBuffer::Clear() {
        m_Data.clear();
        m_Position = 0;
    }

    std::string PlayerSave::FormatUsername(const std::string& username) {

        std::string formatted = username;
        std::transform(formatted.begin(), formatted.end(), formatted.begin(), ::tolower);
        return formatted;
    }

    std::string PlayerSave::GetPlayerFilePath(const std::string& username) {
        return std::string(SAVE_DIRECTORY) + FormatUsername(username) + ".dat.gz";
    }

    bool PlayerSave::PlayerExists(const std::string& username) {
        std::string filepath = GetPlayerFilePath(username);
        return std::filesystem::exists(filepath);
    }

    bool PlayerSave::VerifyPassword(const std::string& username, const std::string& password) {
        if (!PlayerExists(username)) {
            return false;
        }

        try {
            std::vector<uint8_t> data;
            if (!ReadCompressedFile(GetPlayerFilePath(username), data)) {
                return false;
            }

            SerializationBuffer buffer;
            buffer.SetData(data);

            std::string savedUsername = buffer.ReadString();
            std::string savedPassword = buffer.ReadString();

            return savedPassword == password;
        } catch (const std::exception& e) {
            LOG_ERROR("[SAVE] Error verifying password: {}", e.what());
            return false;
        }
    }

    void PlayerSave::SerializePlayer(SerializationBuffer& buffer, const Player& player, const std::string& password) {

        buffer.WriteString(player.GetUsername());
        buffer.WriteString(password);

        buffer.WriteByte(0);  // rights

        buffer.WriteByte(1);  // members

        LOG_INFO("[SAVE] Saving position: ({}, {}, {})",
                 player.GetPosition().GetX(),
                 player.GetPosition().GetY(),
                 player.GetPosition().GetZ());
        buffer.WriteShort(player.GetPosition().GetX());
        buffer.WriteShort(player.GetPosition().GetY());
        buffer.WriteByte(player.GetPosition().GetZ());

        // Save appearance data
        const auto& colors = player.GetColors();
        buffer.WriteByte(player.GetGender());  // Save actual gender
        for (size_t i = 0; i < colors.size(); i++) {
            buffer.WriteByte(static_cast<uint8_t>(colors[i]));
        }

        const auto& appearance = player.GetAppearance();
        for (size_t i = 0; i < appearance.size(); i++) {
            buffer.WriteByte(static_cast<uint8_t>(appearance[i]));
        }

        const auto& equipment = player.GetEquipment();
        for (size_t i = 0; i < equipment.size(); i++) {
            uint32_t itemId = equipment[i];
            if (itemId == 0) {
                buffer.WriteShort(65535);
            } else {
                buffer.WriteShort(static_cast<uint16_t>(itemId));
                buffer.WriteInt(1);
            }
        }

        for (int i = 0; i < 23; i++) {
            buffer.WriteByte(99);
            buffer.WriteDouble(13034431.0);
        }

        for (int i = 0; i < 28; i++) {
            buffer.WriteShort(65535);
        }

        for (int i = 0; i < 400; i++) {
            buffer.WriteShort(65535);
        }

        LOG_INFO("[SAVE] Serialized player data: {} bytes", buffer.Size());
    }

    void PlayerSave::DeserializePlayer(SerializationBuffer& buffer, Player& player, std::string& password) {

        std::string username = buffer.ReadString();
        password = buffer.ReadString();

        uint8_t rights = buffer.ReadByte();

        uint8_t members = buffer.ReadByte();

        uint16_t x = buffer.ReadShort();
        uint16_t y = buffer.ReadShort();
        uint8_t z = buffer.ReadByte();
        player.GetPosition() = Position(x, y, z);

        uint8_t gender = buffer.ReadByte();
        player.SetGender(gender);

        // Read and restore colors (5 bytes)
        std::array<uint32_t, 5> colors;
        for (int i = 0; i < 5; i++) {
            colors[i] = buffer.ReadByte();
        }
        player.SetColors(colors);

        // Read and restore appearance (7 bytes)
        std::array<uint32_t, 7> appearance;
        for (int i = 0; i < 7; i++) {
            appearance[i] = buffer.ReadByte();
        }
        player.SetAppearanceArray(appearance);

        LOG_INFO("[SAVE] Restored appearance for {}: gender={}, colors=[{},{},{},{},{}], appearance=[{},{},{},{},{},{},{}]",
                 username, gender, colors[0], colors[1], colors[2], colors[3], colors[4],
                 appearance[0], appearance[1], appearance[2], appearance[3], appearance[4], appearance[5], appearance[6]);

        for (int i = 0; i < 14; i++) {
            uint16_t itemId = buffer.ReadShort();
            if (itemId != 65535) {
                uint32_t count = buffer.ReadInt();

            }
        }

        for (int i = 0; i < 23; i++) {
            uint8_t level = buffer.ReadByte();
            double experience = buffer.ReadDouble();

        }

        for (int i = 0; i < 28; i++) {
            uint16_t itemId = buffer.ReadShort();
            if (itemId != 65535) {
                uint32_t count = buffer.ReadInt();

            }
        }

        if (buffer.HasRemaining()) {
            for (int i = 0; i < 400; i++) {
                uint16_t itemId = buffer.ReadShort();
                if (itemId != 65535) {
                    uint32_t count = buffer.ReadInt();

                }
            }
        }

        LOG_INFO("[SAVE] Deserialized player: {} at ({}, {}, {})", username, x, y, z);
    }

    bool PlayerSave::WriteCompressedFile(const std::string& filename, const std::vector<uint8_t>& data) {
        try {

            std::filesystem::path filepath(filename);
            std::filesystem::create_directories(filepath.parent_path());

            uLongf compressedSize = compressBound(data.size());
            std::vector<uint8_t> compressed(compressedSize);

            int result = compress(compressed.data(), &compressedSize, data.data(), data.size());
            if (result != Z_OK) {
                LOG_ERROR("[SAVE] GZIP compression failed: {}", result);
                return false;
            }

            compressed.resize(compressedSize);

            std::ofstream file(filename, std::ios::binary);
            if (!file) {
                LOG_ERROR("[SAVE] Failed to open file for writing: {}", filename);
                return false;
            }

            file.write(reinterpret_cast<const char*>(compressed.data()), compressed.size());
            file.close();

            LOG_INFO("[SAVE] Saved compressed file: {} ({} -> {} bytes)", filename, data.size(), compressed.size());
            return true;

        } catch (const std::exception& e) {
            LOG_ERROR("[SAVE] Exception writing file: {}", e.what());
            return false;
        }
    }

    bool PlayerSave::ReadCompressedFile(const std::string& filename, std::vector<uint8_t>& data) {
        try {

            std::ifstream file(filename, std::ios::binary | std::ios::ate);
            if (!file) {
                LOG_ERROR("[SAVE] Failed to open file for reading: {}", filename);
                return false;
            }

            std::streamsize compressedSize = file.tellg();
            file.seekg(0, std::ios::beg);

            std::vector<uint8_t> compressed(compressedSize);
            if (!file.read(reinterpret_cast<char*>(compressed.data()), compressedSize)) {
                LOG_ERROR("[SAVE] Failed to read file: {}", filename);
                return false;
            }
            file.close();

            uLongf uncompressedSize = std::max<uLongf>(compressedSize * 20, 8192);
            data.resize(uncompressedSize);

            int result = uncompress(data.data(), &uncompressedSize, compressed.data(), compressedSize);

            if (result == Z_BUF_ERROR) {
                uncompressedSize = std::max<uLongf>(compressedSize * 100, 32768);
                data.resize(uncompressedSize);
                result = uncompress(data.data(), &uncompressedSize, compressed.data(), compressedSize);
            }

            if (result != Z_OK) {
                LOG_ERROR("[SAVE] GZIP decompression failed: {}", result);
                return false;
            }

            data.resize(uncompressedSize);
            LOG_INFO("[SAVE] Loaded compressed file: {} ({} -> {} bytes)", filename, compressedSize, uncompressedSize);
            return true;

        } catch (const std::exception& e) {
            LOG_ERROR("[SAVE] Exception reading file: {}", e.what());
            return false;
        }
    }

    bool PlayerSave::SavePlayer(const Player& player, const std::string& password) {
        try {
            SerializationBuffer buffer;
            SerializePlayer(buffer, player, password);

            std::string filepath = GetPlayerFilePath(player.GetUsername());
            return WriteCompressedFile(filepath, buffer.GetData());

        } catch (const std::exception& e) {
            LOG_ERROR("[SAVE] Exception saving player {}: {}", player.GetUsername(), e.what());
            return false;
        }
    }

    bool PlayerSave::LoadPlayer(Player& player, std::string& password) {
        try {
            std::string filepath = GetPlayerFilePath(player.GetUsername());

            if (!PlayerExists(player.GetUsername())) {
                LOG_INFO("[SAVE] No saved data for player: {}", player.GetUsername());
                return false;
            }

            std::vector<uint8_t> data;
            if (!ReadCompressedFile(filepath, data)) {
                return false;
            }

            SerializationBuffer buffer;
            buffer.SetData(data);
            DeserializePlayer(buffer, player, password);

            return true;

        } catch (const std::exception& e) {
            LOG_ERROR("[SAVE] Exception loading player {}: {}", player.GetUsername(), e.what());
            return false;
        }
    }

}
