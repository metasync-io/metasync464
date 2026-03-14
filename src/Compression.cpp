#include "epch.h"
#include "Compression.h"
#include "Log.h"
#include <zlib.h>
#include <bzlib.h>

namespace Skeleton {

bool Compression::DecompressGZip(const std::vector<uint8_t>& compressed, std::vector<uint8_t>& decompressed)
{
    if (compressed.size() < 27)
    {
        LOG_ERROR("GZip data too small: {} bytes", compressed.size());
        return false;
    }

    int decompressedLength = ((compressed[5] & 0xFF) << 24) |
                            ((compressed[6] & 0xFF) << 16) |
                            ((compressed[7] & 0xFF) << 8) |
                            (compressed[8] & 0xFF);

    decompressed.resize(decompressedLength);

    z_stream stream;
    stream.zalloc = Z_NULL;
    stream.zfree = Z_NULL;
    stream.opaque = Z_NULL;
    stream.avail_in = compressed.size() - 27;
    stream.next_in = const_cast<Bytef*>(&compressed[19]);
    stream.avail_out = decompressedLength;
    stream.next_out = &decompressed[0];

    if (inflateInit2(&stream, -15) != Z_OK)
    {
        LOG_ERROR("Failed to initialize GZip decompression");
        return false;
    }

    int result = inflate(&stream, Z_FINISH);
    inflateEnd(&stream);

    if (result != Z_STREAM_END)
    {
        LOG_ERROR("GZip decompression failed: {}", result);
        return false;
    }

    decompressed.resize(stream.total_out);
    return true;
}

bool Compression::DecompressBZip2(const std::vector<uint8_t>& compressed, std::vector<uint8_t>& decompressed)
{
    if (compressed.size() < 14)
    {
        LOG_ERROR("BZip2 data too small: {} bytes", compressed.size());
        return false;
    }

    int compressedSize = ((compressed[1] & 0xFF) << 24) |
                        ((compressed[2] & 0xFF) << 16) |
                        ((compressed[3] & 0xFF) << 8) |
                        (compressed[4] & 0xFF);

    int decompressedLength = ((compressed[5] & 0xFF) << 24) |
                            ((compressed[6] & 0xFF) << 16) |
                            ((compressed[7] & 0xFF) << 8) |
                            (compressed[8] & 0xFF);

    decompressed.resize(decompressedLength);

    bz_stream strm;
    strm.bzalloc = nullptr;
    strm.bzfree = nullptr;
    strm.opaque = nullptr;

    const char* BZIP_HEADER = "BZh1";
    strm.avail_in = 4;
    strm.next_in = const_cast<char*>(BZIP_HEADER);
    strm.avail_out = decompressed.size();
    strm.next_out = reinterpret_cast<char*>(&decompressed[0]);

    if (BZ2_bzDecompressInit(&strm, 0, 0) != BZ_OK)
    {
        LOG_ERROR("Failed to initialize BZip2 decompression");
        return false;
    }

    int result = BZ2_bzDecompress(&strm);

    strm.avail_in = compressed.size() - 9;
    strm.next_in = const_cast<char*>(reinterpret_cast<const char*>(&compressed[9]));
    result = BZ2_bzDecompress(&strm);

    int actualSize = strm.total_out_lo32;
    BZ2_bzDecompressEnd(&strm);

    if (result != BZ_STREAM_END && result != BZ_OK)
    {
        LOG_ERROR("BZip2 decompression failed: {}", result);
        return false;
    }

    decompressed.resize(actualSize);
    return true;
}

bool Compression::Decompress(uint8_t compressionType, const std::vector<uint8_t>& compressed, std::vector<uint8_t>& decompressed)
{
    if (compressionType == 0)
    {
        decompressed = compressed;
        return true;
    }
    else if (compressionType == 1)
    {
        return DecompressBZip2(compressed, decompressed);
    }
    else if (compressionType == 2)
    {
        return DecompressGZip(compressed, decompressed);
    }
    else
    {
        LOG_ERROR("Unknown compression type: {}", compressionType);
        return false;
    }
}

bool Compression::CompressGZip(const std::vector<int8_t>& input, int offset, int length, std::vector<int8_t>& output)
{
    z_stream stream;
    stream.zalloc = Z_NULL;
    stream.zfree = Z_NULL;
    stream.opaque = Z_NULL;
    stream.avail_in = length;
    stream.next_in = reinterpret_cast<Bytef*>(const_cast<int8_t*>(&input[offset]));

    output.resize(length + 1024);
    stream.avail_out = output.size();
    stream.next_out = reinterpret_cast<Bytef*>(&output[0]);

    if (deflateInit2(&stream, Z_DEFAULT_COMPRESSION, Z_DEFLATED, 15 + 16, 8, Z_DEFAULT_STRATEGY) != Z_OK)
    {
        LOG_ERROR("Failed to initialize GZIP compression");
        return false;
    }

    int result = deflate(&stream, Z_FINISH);
    deflateEnd(&stream);

    if (result != Z_STREAM_END)
    {
        LOG_ERROR("GZIP compression failed: {}", result);
        return false;
    }

    output.resize(stream.total_out);
    return true;
}

}
