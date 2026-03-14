#include "epch.h"
#include "UpdateServer.h"
#include "Compression.h"
#include "Log.h"

namespace Skeleton {

std::unique_ptr<std::fstream> UpdateServer::m_DataFile = nullptr;
std::unique_ptr<std::fstream> UpdateServer::m_IndexFiles[32] = {};
std::unique_ptr<std::fstream> UpdateServer::m_CrcFile = nullptr;
std::vector<uint8_t> UpdateServer::m_CrcTable;

bool UpdateServer::Initialize()
{
    LOG_INFO("Loading cache files...");

    try
    {
        m_DataFile = std::make_unique<std::fstream>("./data/cache/main_file_cache.dat2",
            std::ios::in | std::ios::binary);

        if (!m_DataFile->is_open())
        {
            LOG_ERROR("Failed to open main_file_cache.dat2");
            return false;
        }

        for (int i = 0; i < 32; i++)
        {
            std::string filename = "./data/cache/main_file_cache.idx" + std::to_string(i);
            std::ifstream testFile(filename);

            if (testFile.good())
            {
                testFile.close();
                m_IndexFiles[i] = std::make_unique<std::fstream>(filename,
                    std::ios::in | std::ios::binary);

                if (!m_IndexFiles[i]->is_open())
                {
                    LOG_WARN("Failed to open {}", filename);
                }
            }
        }

        m_CrcFile = std::make_unique<std::fstream>("./data/cache/main_file_cache.idx255",
            std::ios::in | std::ios::binary);

        if (!m_CrcFile->is_open())
        {
            LOG_WARN("Failed to open main_file_cache.idx255");
        }

        LOG_INFO("Cache files loaded successfully");

        GenerateCrcTable();

        return true;
    }
    catch (const std::exception& e)
    {
        LOG_ERROR("Exception loading cache: {}", e.what());
        return false;
    }
}

uint32_t UpdateServer::CalculateCrc32(const std::vector<uint8_t>& data)
{
    uint32_t crc = 0xFFFFFFFF;

    for (uint8_t byte : data)
    {
        crc ^= byte;
        for (int i = 0; i < 8; i++)
        {
            if (crc & 1)
                crc = (crc >> 1) ^ 0xEDB88320;
            else
                crc = crc >> 1;
        }
    }

    return ~crc;
}

void UpdateServer::GenerateCrcTable()
{
    LOG_INFO("Generating CRC reference table...");

    int cacheLength = GetCacheLength(255);

    m_CrcTable.clear();
    m_CrcTable.reserve(4048);

    m_CrcTable.push_back(0);

    int length = cacheLength * 4;
    m_CrcTable.push_back((length >> 24) & 0xFF);
    m_CrcTable.push_back((length >> 16) & 0xFF);
    m_CrcTable.push_back((length >> 8) & 0xFF);
    m_CrcTable.push_back(length & 0xFF);

    LOG_INFO("CRC table header: compression=0, length={} (0x{:08X})", length, length);

    for (int i = 0; i < cacheLength; i++)
    {
        std::vector<uint8_t> fileData;
        if (ReadFileData(255, i, fileData))
        {
            uint32_t crc = CalculateCrc32(fileData);
            m_CrcTable.push_back((crc >> 24) & 0xFF);
            m_CrcTable.push_back((crc >> 16) & 0xFF);
            m_CrcTable.push_back((crc >> 8) & 0xFF);
            m_CrcTable.push_back(crc & 0xFF);

            if (i < 3)
            {
                LOG_INFO("  Cache 255 file {}: size={} bytes, CRC=0x{:08X}", i, fileData.size(), crc);
            }
        }
        else
        {
            for (int j = 0; j < 4; j++)
            {
                m_CrcTable.push_back(0);
            }
        }
    }

    LOG_INFO("CRC table generated: {} bytes for {} cache files", m_CrcTable.size(), cacheLength);
}

void UpdateServer::Shutdown()
{
    if (m_DataFile)
    {
        m_DataFile->close();
        m_DataFile.reset();
    }

    if (m_CrcFile)
    {
        m_CrcFile->close();
        m_CrcFile.reset();
    }

    for (int i = 0; i < 32; i++)
    {
        if (m_IndexFiles[i])
        {
            m_IndexFiles[i]->close();
            m_IndexFiles[i].reset();
        }
    }
}

int UpdateServer::GetCacheLength(int cache)
{
    try
    {
        if (cache == 255 && m_CrcFile && m_CrcFile->is_open())
        {
            m_CrcFile->seekg(0, std::ios::end);
            return static_cast<int>(m_CrcFile->tellg()) / 6;
        }
        else if (cache >= 0 && cache < 32 && m_IndexFiles[cache] && m_IndexFiles[cache]->is_open())
        {
            m_IndexFiles[cache]->seekg(0, std::ios::end);
            return static_cast<int>(m_IndexFiles[cache]->tellg()) / 6;
        }
    }
    catch (const std::exception& e)
    {
        LOG_ERROR("Exception in GetCacheLength: {}", e.what());
    }

    return 0;
}

bool UpdateServer::ReadFileData(int cache, int id, std::vector<uint8_t>& outData)
{
    try
    {
        std::fstream* indexFile = nullptr;

        if (cache == 255)
        {
            indexFile = m_CrcFile.get();
        }
        else if (cache >= 0 && cache < 32)
        {
            indexFile = m_IndexFiles[cache].get();
        }

        if (!indexFile || !indexFile->is_open() || !m_DataFile || !m_DataFile->is_open())
        {
            return false;
        }

        uint8_t header[6];
        indexFile->seekg(id * 6);
        indexFile->read(reinterpret_cast<char*>(header), 6);

        if (indexFile->gcount() != 6)
        {
            return false;
        }

        int length = ((header[0] & 0xFF) << 16) | ((header[1] & 0xFF) << 8) | (header[2] & 0xFF);
        int sector = ((header[3] & 0xFF) << 16) | ((header[4] & 0xFF) << 8) | (header[5] & 0xFF);

        if (sector <= 0 || length <= 0)
        {
            return false;
        }

        outData.clear();
        outData.reserve(length);

        int read = 0;
        int chunk = 0;

        while (read < length)
        {
            if (sector == 0)
            {
                return false;
            }

            m_DataFile->seekg(sector * 520);

            uint8_t chunkHeader[8];
            m_DataFile->read(reinterpret_cast<char*>(chunkHeader), 8);

            if (m_DataFile->gcount() != 8)
            {
                return false;
            }

            int nextSector = ((chunkHeader[4] & 0xFF) << 16) |
                           ((chunkHeader[5] & 0xFF) << 8) |
                           (chunkHeader[6] & 0xFF);

            int chunkSize = std::min(512, length - read);

            for (int i = 0; i < chunkSize; i++)
            {
                uint8_t byte;
                m_DataFile->read(reinterpret_cast<char*>(&byte), 1);

                if (m_DataFile->gcount() != 1)
                {
                    return false;
                }

                outData.push_back(byte);
                read++;
            }

            sector = nextSector;
            chunk++;
        }

        return true;
    }
    catch (const std::exception& e)
    {
        LOG_ERROR("Exception in ReadFileData: {}", e.what());
        return false;
    }
}

bool UpdateServer::GetCacheFile(int cache, int id, std::vector<uint8_t>& outData)
{
    if (cache == 255 && id == 255)
    {
        outData = m_CrcTable;
        return !outData.empty();
    }

    return ReadFileData(cache, id, outData);
}

StreamBuffer UpdateServer::CreateCacheResponse(int cache, int id)
{
    std::vector<uint8_t> fileData;

    if (!GetCacheFile(cache, id, fileData) || fileData.empty())
    {
        LOG_WARN("Failed to get cache file [{}, {}] - sending empty response", cache, id);
        StreamBuffer outStream(8);
        outStream.WriteByte(cache);
        outStream.WriteShort(id);
        outStream.WriteByte(0);
        outStream.WriteInt(0);
        outStream.WriteByte(0);
        return outStream;
    }

    LOG_INFO("CreateCacheResponse: cache={}, id={}, fileData.size()={}", cache, id, fileData.size());

    if (fileData.size() >= 10)
    {
        LOG_INFO("  First 10 bytes: {:02X} {:02X} {:02X} {:02X} {:02X} {:02X} {:02X} {:02X} {:02X} {:02X}",
                 fileData[0], fileData[1], fileData[2], fileData[3], fileData[4],
                 fileData[5], fileData[6], fileData[7], fileData[8], fileData[9]);
    }

    int len;

    if (cache == 255 && id == 255)
    {
        len = static_cast<int>(fileData.size());
        LOG_INFO("  CRC table: len={}", len);
    }
    else
    {
        if (fileData.size() < 5)
        {
            LOG_ERROR("Invalid cache file data size: {} for cache={} id={}", fileData.size(), cache, id);
            StreamBuffer outStream(8);
            outStream.WriteByte(cache);
            outStream.WriteShort(id);
            outStream.WriteByte(0);
            outStream.WriteInt(0);
            outStream.WriteByte(0);
            return outStream;
        }

        int lengthField = ((fileData[1] & 0xFF) << 24) +
                          ((fileData[2] & 0xFF) << 16) +
                          ((fileData[3] & 0xFF) << 8) +
                          (fileData[4] & 0xFF);

        len = lengthField + 9;

        if (fileData[0] == 0)
        {
            len -= 4;
        }

        LOG_INFO("  Regular file: compression={}, lengthField={}, calculated len={}",
                 fileData[0], lengthField, len);
    }

    StreamBuffer outStream(len + 100);
    outStream.WriteByte(cache);
    outStream.WriteShort(id);

    LOG_INFO("  Packet header bytes: {:02X} {:02X} {:02X} (cache={}, id={})",
             cache, (id >> 8) & 0xFF, id & 0xFF, cache, id);

    int c = 3;
    int ff_markers = 0;
    for (int i = 0; i < len && i < static_cast<int>(fileData.size()); i++)
    {
        if (c == 512)
        {
            outStream.WriteByte(0xFF);
            c = 1;
            ff_markers++;
        }
        outStream.WriteByte(fileData[i]);
        c++;
    }

    LOG_INFO("  Wrote {} data bytes, {} FF markers, total={} bytes", len, ff_markers, outStream.Position());

    const auto& data = outStream.Data();
    if (data.size() >= 20)
    {
        LOG_INFO("  First 20 bytes of packet: {:02X} {:02X} {:02X} {:02X} {:02X} {:02X} {:02X} {:02X} {:02X} {:02X} {:02X} {:02X} {:02X} {:02X} {:02X} {:02X} {:02X} {:02X} {:02X} {:02X}",
                 (uint8_t)data[0], (uint8_t)data[1], (uint8_t)data[2], (uint8_t)data[3], (uint8_t)data[4],
                 (uint8_t)data[5], (uint8_t)data[6], (uint8_t)data[7], (uint8_t)data[8], (uint8_t)data[9],
                 (uint8_t)data[10], (uint8_t)data[11], (uint8_t)data[12], (uint8_t)data[13], (uint8_t)data[14],
                 (uint8_t)data[15], (uint8_t)data[16], (uint8_t)data[17], (uint8_t)data[18], (uint8_t)data[19]);
    }

    return outStream;
}

}
