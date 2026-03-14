#pragma once
#include <cstdint>

class ISAACCipher {
public:

    explicit ISAACCipher(const uint32_t ai[4]);

    ~ISAACCipher() = default;

    uint32_t GetNextKey();

private:

    void InitializeKeySet();

    void GenerateNextKeySet();

private:

    static constexpr int SIZE = 256;

    uint32_t m_KeySetArray[SIZE]{};

    uint32_t m_CryptArray[SIZE]{};

    uint32_t m_CryptVar1 = 0;
    uint32_t m_CryptVar2 = 0;
    uint32_t m_CryptVar3 = 0;

    uint32_t m_KeyArrayIdx = 0;
};
