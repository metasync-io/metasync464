#pragma once
#include "epch.h"
#include "ISAACCipher.h"

namespace Skeleton {

    enum class ByteOrder {
        LITTLE,
        BIG,
        MIDDLE,
        INVERSE_MIDDLE
    };

    enum class ValueType {
        STANDARD,
        A,
        C,
        S
    };

    enum class AccessType {
        BYTE_ACCESS,
        BIT_ACCESS
    };

    enum class VariableHeaderSize {
        NONE, BYTE, SHORT
    };

    static constexpr std::array<int32_t, 33> BIT_MASK = {
        0, 0x1, 0x3, 0x7, 0xf, 0x1f, 0x3f, 0x7f, 0xff,
        0x1ff, 0x3ff, 0x7ff, 0xfff, 0x1fff, 0x3fff, 0x7fff, 0xffff,
        0x1ffff, 0x3ffff, 0x7ffff, 0xfffff, 0x1fffff, 0x3fffff,
        0x7fffff, 0xffffff, 0x1ffffff, 0x3ffffff, 0x7ffffff,
        0xfffffff, 0x1fffffff, 0x3fffffff, 0x7fffffff, -1
    };

    constexpr size_t DEFAULT_BUFFER_SIZE = 512;

    class StreamBuffer
    {
    public:

        StreamBuffer();
        StreamBuffer(int32_t size);
        StreamBuffer(const int8_t* data, int32_t length);

        void SetAccessType(AccessType type);

        void Clear();

        int32_t Position() const { return m_Pos; }
        const AccessType GetAccessType() const { return m_AccessType; }
        const int32_t GetBitPosition() const { return m_BitPosition; }
        const bool HasRemaining() const { return m_Pos < m_Buffer.size(); }
        const int32_t Remaining() const { return m_Buffer.size() - m_Pos; }
        const std::vector<int8_t>& Data() const { return m_Buffer; }

        int32_t ReadByte(bool isSigned, ValueType type);
        int32_t ReadByte() { return ReadByte(true, ValueType::STANDARD); }
        int32_t ReadByte(bool isSigned) { return ReadByte(isSigned, ValueType::STANDARD); }
        int32_t ReadByte(ValueType type) { return ReadByte(true, type); }

        int32_t ReadShort(bool isSigned, ValueType type, ByteOrder order);
        int32_t ReadShort() { return ReadShort(true, ValueType::STANDARD, ByteOrder::BIG); }
        int32_t ReadShort(bool isSigned) { return ReadShort(isSigned, ValueType::STANDARD, ByteOrder::BIG); }
        int32_t ReadShort(ValueType type) { return ReadShort(true, type, ByteOrder::BIG); }
        int32_t ReadShort(bool isSigned, ValueType type) { return ReadShort(isSigned, type, ByteOrder::BIG); }
        int32_t ReadShort(ByteOrder order) { return ReadShort(true, ValueType::STANDARD, order); }
        int32_t ReadShort(bool isSigned, ByteOrder order) { return ReadShort(isSigned, ValueType::STANDARD, order); }
        int32_t ReadShort(ValueType type, ByteOrder order) { return ReadShort(true, type, order); }

        int32_t ReadInt(bool isSigned, ValueType type, ByteOrder order);
        int32_t ReadInt() { return ReadInt(true, ValueType::STANDARD, ByteOrder::BIG); }
        int32_t ReadInt(bool isSigned) { return ReadInt(isSigned, ValueType::STANDARD, ByteOrder::BIG);}
        int32_t ReadInt(ValueType type) { return ReadInt(true, type, ByteOrder::BIG); }
        int32_t ReadInt(bool isSigned, ValueType type) { return ReadInt(isSigned, type, ByteOrder::BIG);}
        int32_t ReadInt(ByteOrder order) { return ReadInt(true, ValueType::STANDARD, order); }
        int32_t ReadInt(bool isSigned, ByteOrder order) { return ReadInt(isSigned, ValueType::STANDARD, order); }
        int32_t ReadInt(ValueType type, ByteOrder order) { return ReadInt(true, type, order); }

        int64_t ReadLong(ValueType type, ByteOrder order);
        int64_t ReadLong() { return ReadLong(ValueType::STANDARD, ByteOrder::BIG); }
        int64_t ReadLong(ValueType type) {return ReadLong(type, ByteOrder::BIG); }
        int64_t ReadLong(ByteOrder order) { return ReadLong(ValueType::STANDARD, order); }

        std::string ReadString();

        void WriteByte(int32_t value, ValueType type);
        void WriteByte(int32_t value) { WriteByte(value, ValueType::STANDARD); }
        void WriteBytes(const StreamBuffer& other);

        void WriteShort(int32_t value, ValueType type, ByteOrder order);
        void WriteShort(int32_t value) { WriteShort(value, ValueType::STANDARD, ByteOrder::BIG); }
        void WriteShort(int32_t value, ValueType type) { WriteShort(value, type, ByteOrder::BIG); }
        void WriteShort(int32_t value, ByteOrder order) { WriteShort(value, ValueType::STANDARD, order); }

        void WriteInt(int32_t value, ValueType type, ByteOrder order);
        void WriteInt(int32_t value) { WriteInt(value, ValueType::STANDARD, ByteOrder::BIG); }
        void WriteInt(int32_t value, ValueType type) { WriteInt(value, type, ByteOrder::BIG); }
        void WriteInt(int32_t value, ByteOrder order) { WriteInt(value, ValueType::STANDARD, order); }

        void WriteLong(int64_t value, ValueType type, ByteOrder order);
        void WriteLong(int32_t value) { WriteLong(value, ValueType::STANDARD, ByteOrder::BIG); }
        void WriteLong(int32_t value, ValueType type) { WriteLong(value, type, ByteOrder::BIG); }
        void WriteLong(int32_t value, ByteOrder order) { WriteLong(value, ValueType::STANDARD, order); }

        void WriteString(const std::string& string);

        void WriteHeader(int32_t value, ISAACCipher& cipher, VariableHeaderSize headerSize = VariableHeaderSize::NONE);
        void FinishVariableHeader();

        void WriteBits(int32_t amount, int32_t value);
        void WriteBit(bool flag) { WriteBits(1, flag ? 1 : 0); }

        void WriteFrameSizeShort(int32_t size);
        void WriteFrameSizeByte(int32_t size);

        void WriteTriByte(int32_t value);
        void WriteSmart(int32_t value);
        void WriteSignedSmart(int32_t value);
        void WriteReverse(const int8_t* data, int32_t offset, int32_t length);
        void WriteReverseA(const int8_t* data, int32_t offset, int32_t length);
        void Skip(int32_t count);

    private:

        AccessType m_AccessType = AccessType::BYTE_ACCESS;
        int32_t m_BitPosition = 0;
        int32_t m_Pos = 0;
        int32_t m_FrameSize = 0;
        VariableHeaderSize m_HeaderSize = VariableHeaderSize::NONE;

        std::vector<int8_t> m_Buffer;

    };
}
