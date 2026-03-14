#pragma once
#include "epch.h"
#include "StreamBuffer.h"

namespace Skeleton {

    class TextUtils {
    public:
        static std::string DecryptPlayerChat(StreamBuffer& buffer, int32_t totalChars);
        static std::string FilterText(const std::string& text);
        static std::string OptimizeText(const std::string& text);
        static void TextPack(uint8_t* packedData, const std::string& text);

    private:
        static const std::vector<int32_t> CHAT_DECRYPT_KEYS;
        static const std::vector<uint8_t> CHAT_BIT_SIZES;
        static const std::vector<int32_t> CHAT_MASKS;
    };

}
