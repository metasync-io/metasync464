#pragma once
#include "epch.h"
#include "StreamBuffer.h"
#include <fstream>
#include <vector>
#include <memory>

namespace Skeleton {

class UpdateServer
{
public:
    static bool Initialize();
    static void Shutdown();
    static bool GetCacheFile(int cache, int id, std::vector<uint8_t>& outData);
    static StreamBuffer CreateCacheResponse(int cache, int id);

private:
    static std::unique_ptr<std::fstream> m_DataFile;
    static std::unique_ptr<std::fstream> m_IndexFiles[32];
    static std::unique_ptr<std::fstream> m_CrcFile;
    static std::vector<uint8_t> m_CrcTable;

    static bool ReadFileData(int cache, int id, std::vector<uint8_t>& outData);
    static int GetCacheLength(int cache);
    static void GenerateCrcTable();
    static uint32_t CalculateCrc32(const std::vector<uint8_t>& data);
};

}
