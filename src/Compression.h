#pragma once
#include <vector>
#include <cstdint>

namespace Skeleton {

class Compression
{
public:
    static bool DecompressGZip(const std::vector<uint8_t>& compressed, std::vector<uint8_t>& decompressed);
    static bool DecompressBZip2(const std::vector<uint8_t>& compressed, std::vector<uint8_t>& decompressed);
    static bool Decompress(uint8_t compressionType, const std::vector<uint8_t>& compressed, std::vector<uint8_t>& decompressed);
    static bool CompressGZip(const std::vector<int8_t>& input, int offset, int length, std::vector<int8_t>& output);
};

}
