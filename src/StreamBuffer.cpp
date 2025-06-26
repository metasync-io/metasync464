#include "epch.h"
#include "StreamBuffer.h"

namespace Skeleton {

	StreamBuffer::StreamBuffer() : m_Buffer(DEFAULT_BUFFER_SIZE) {}

	StreamBuffer::StreamBuffer(int32_t size) : m_Buffer(size) {}

	StreamBuffer::StreamBuffer(const int8_t* data, int32_t length)
		: m_Buffer(data, data + length) {
	}

	void StreamBuffer::SetAccessType(AccessType type)
	{
		m_AccessType = type;
		switch (type) {
		case AccessType::BIT_ACCESS:
			m_BitPosition = m_Pos * 8;
			break;
		case AccessType::BYTE_ACCESS:
			m_Pos = (m_BitPosition + 7) / 8;
			break;
		}
	}

	void StreamBuffer::Clear()
	{
		m_Buffer.clear();
		m_Pos = 0;
	}

	int32_t StreamBuffer::ReadByte(bool isSigned, ValueType type)
	{
		int8_t raw = m_Buffer[m_Pos++];
		int32_t value = static_cast<int32_t>(raw);

		switch (type) {
			case ValueType::A: value -= 128; break;
			case ValueType::C: value = -value; break;
			case ValueType::S: value = 128 - value; break;
			case ValueType::STANDARD: break;
		}

		return isSigned ? value : (value & 0xFF);
	}

	int32_t StreamBuffer::ReadShort(bool isSigned, ValueType type, ByteOrder order)
	{
		int32_t high = 0, low = 0;

		switch (order) {
		case ByteOrder::BIG:
			high = ReadByte(false) << 8;
			low = ReadByte(false, type);
			break;
		case ByteOrder::LITTLE:
			low = ReadByte(false, type);
			high = ReadByte(false) << 8;
			break;
		default:
			throw std::invalid_argument("Unsupported ByteOrder for ReadShort");
		}

		int32_t value = high | low;
		return isSigned ? value : (value & 0xFFFF);
	}

	int32_t StreamBuffer::ReadInt(bool isSigned, ValueType type, ByteOrder order) 
	{
		int32_t b1, b2, b3, b4;

		switch (order) {
		case ByteOrder::BIG:
			b1 = ReadByte(false) << 24;
			b2 = ReadByte(false) << 16;
			b3 = ReadByte(false) << 8;
			b4 = ReadByte(false, type);
			break;
		case ByteOrder::MIDDLE:
			b1 = ReadByte(false) << 8;
			b2 = ReadByte(false, type);
			b3 = ReadByte(false) << 24;
			b4 = ReadByte(false) << 16;
			break;
		case ByteOrder::INVERSE_MIDDLE:
			b1 = ReadByte(false) << 16;
			b2 = ReadByte(false) << 24;
			b3 = ReadByte(false, type);
			b4 = ReadByte(false) << 8;
			break;
		case ByteOrder::LITTLE:
			b1 = ReadByte(false, type);
			b2 = ReadByte(false) << 8;
			b3 = ReadByte(false) << 16;
			b4 = ReadByte(false) << 24;
			break;
		default:
			throw std::invalid_argument("Unsupported ByteOrder for ReadInt");
		}

		int32_t value = b1 | b2 | b3 | b4;
		return isSigned ? value : static_cast<uint32_t>(value);
	}

	int64_t StreamBuffer::ReadLong(ValueType type, ByteOrder order)
	{
		int64_t value = 0;
		switch (order) {
		case ByteOrder::BIG:
			value |= static_cast<int64_t>(ReadByte(false)) << 56;
			value |= static_cast<int64_t>(ReadByte(false)) << 48;
			value |= static_cast<int64_t>(ReadByte(false)) << 40;
			value |= static_cast<int64_t>(ReadByte(false)) << 32;
			value |= static_cast<int64_t>(ReadByte(false)) << 24;
			value |= static_cast<int64_t>(ReadByte(false)) << 16;
			value |= static_cast<int64_t>(ReadByte(false)) << 8;
			value |= static_cast<int64_t>(ReadByte(false, type));
			break;

		case ByteOrder::LITTLE:
			value |= static_cast<int64_t>(ReadByte(false, type));
			value |= static_cast<int64_t>(ReadByte(false)) << 8;
			value |= static_cast<int64_t>(ReadByte(false)) << 16;
			value |= static_cast<int64_t>(ReadByte(false)) << 24;
			value |= static_cast<int64_t>(ReadByte(false)) << 32;
			value |= static_cast<int64_t>(ReadByte(false)) << 40;
			value |= static_cast<int64_t>(ReadByte(false)) << 48;
			value |= static_cast<int64_t>(ReadByte(false)) << 56;
			break;

		default:
			throw std::invalid_argument("Unsupported ByteOrder for ReadLong");
		}

		return value;
	}

	std::string StreamBuffer::ReadString()
	{
		std::string result;
		uint8_t byte;
		const size_t maxLength = 1024; // Prevent malformed or malicious input
		size_t count = 0;

		while ((byte = static_cast<uint8_t>(ReadByte(false))) != 10) 
		{
			result += static_cast<char>(byte);
			if (++count > maxLength) {
				throw std::runtime_error("StreamBuffer::ReadString exceeded max allowed length");
			}
		}
		return result;
	}

	void StreamBuffer::WriteByte(int32_t value, ValueType type)
	{
		if (m_AccessType != AccessType::BYTE_ACCESS) {
			throw std::invalid_argument("Illegal access type.");
		}
		switch (type) {
			case ValueType::A:
				value += 128;
				break;
			case ValueType::C:
				value = -value;
				break;
			case ValueType::S:
				value = 128 - value;
				break;
		}
		int8_t byteValue = static_cast<int8_t>(value);

		if (m_Pos >= m_Buffer.size()) {
			m_Buffer.resize(m_Pos + 1);
			LOG_WARN("Resize writebuffer!");
		}

		m_Buffer[m_Pos++] = byteValue;
	}

	void StreamBuffer::WriteBytes(const StreamBuffer& other)
	{
		int32_t count = other.m_Pos; // how much data is in 'other'
		if (count == 0) return;

		// Ensure enough capacity
		if (m_Pos + count > m_Buffer.size())
			m_Buffer.resize(m_Pos + count + DEFAULT_BUFFER_SIZE);

		std::memcpy(&m_Buffer[m_Pos], &other.m_Buffer[0], count);
		m_Pos += count;
	}

	void StreamBuffer::WriteShort(int32_t value, ValueType type, ByteOrder order)
	{
		switch (order) {
		case ByteOrder::BIG:
			WriteByte(value >> 8);
			WriteByte(value, type);
			break;
		case ByteOrder::LITTLE:
			WriteByte(value, type);
			WriteByte(value >> 8);
			break;
		default:
			throw std::invalid_argument("Unsupported ByteOrder for WriteShort");
		}
	}

	void StreamBuffer::WriteInt(int32_t value, ValueType type, ByteOrder order)
	{
		switch (order) {
			case ByteOrder::BIG:
				WriteByte(value >> 24);
				WriteByte(value >> 16);
				WriteByte(value >> 8);
				WriteByte(value, type);
				break;
			case ByteOrder::MIDDLE:
				WriteByte(value >> 8);
				WriteByte(value, type);
				WriteByte(value >> 24);
				WriteByte(value >> 16);
				break;
			case ByteOrder::INVERSE_MIDDLE:
				WriteByte(value >> 16);
				WriteByte(value >> 24);
				WriteByte(value, type);
				WriteByte(value >> 8);
				break;
			case ByteOrder::LITTLE:
				WriteByte(value, type);
				WriteByte(value >> 8);
				WriteByte(value >> 16);
				WriteByte(value >> 24);
				break;
			
		}
	}
	
	void StreamBuffer::WriteLong(int64_t value, ValueType type, ByteOrder order)
	{
		switch (order) {
		case ByteOrder::BIG:
			WriteByte((int)(value >> 56));
			WriteByte((int)(value >> 48));
			WriteByte((int)(value >> 40));
			WriteByte((int)(value >> 32));
			WriteByte((int)(value >> 24));
			WriteByte((int)(value >> 16));
			WriteByte((int)(value >> 8));
			WriteByte((int)value, type);
			break;
		case ByteOrder::LITTLE:
			WriteByte((int)value, type);
			WriteByte((int)(value >> 8));
			WriteByte((int)(value >> 16));
			WriteByte((int)(value >> 24));
			WriteByte((int)(value >> 32));
			WriteByte((int)(value >> 40));
			WriteByte((int)(value >> 48));
			WriteByte((int)(value >> 56));
			break;
		default:
			throw std::invalid_argument("Unsupported ByteOrder for WriteLong");
		}
	}

	void StreamBuffer::WriteString(const std::string& string)
	{
		for (uint8_t value : string) {
			WriteByte(value);
		}
		WriteByte(10);
	}

	void StreamBuffer::WriteHeader(int32_t value, ISAACCipher& cipher, VariableHeaderSize headerSize)
	{
		m_HeaderSize = headerSize;
		WriteByte(value + cipher.GetNextKey());
		switch (headerSize)
		{
		case VariableHeaderSize::BYTE:
			m_FrameSize = Position();
			WriteByte(0);
			break;
		case VariableHeaderSize::SHORT:
			m_FrameSize = Position();
			WriteShort(0);
			break;
		}
	}

	void StreamBuffer::FinishVariableHeader() {
		if (m_FrameSize == 0) {
			LOG_WARN("FinishVariableHeader() called with no active frame!");
			return;
		}

		const int32_t payloadSize = Position() - m_FrameSize;
		switch (m_HeaderSize) {
			case VariableHeaderSize::SHORT:
				WriteFrameSizeShort(payloadSize - 2);
				break;
			case VariableHeaderSize::BYTE:
				WriteFrameSizeByte(payloadSize - 1);
				break;
		}

		m_FrameSize = 0;
	}

	void StreamBuffer::WriteFrameSizeShort(int32_t size)
	{
		const size_t offset = m_Pos - size - 2;

		if (m_Pos < static_cast<size_t>(size + 2) || offset + 1 >= m_Buffer.size()) {
			throw std::out_of_range("WriteFrameSizeShort: Invalid offset or size");
		}

		m_Buffer[offset] = static_cast<uint8_t>(size >> 8);
		m_Buffer[offset + 1] = static_cast<uint8_t>(size);
	}


	void StreamBuffer::WriteFrameSizeByte(int32_t size)
	{
		const size_t offset = m_Pos - size - 1;

		if (m_Pos < static_cast<size_t>(size + 1) || offset >= m_Buffer.size()) {
			throw std::out_of_range("WriteFrameSizeByte: Invalid offset or size");
		}

		m_Buffer[offset] = static_cast<uint8_t>(size);
	}

	void StreamBuffer::WriteBits(int32_t amount, int32_t value)
	{
		int32_t bytePos = m_BitPosition >> 3;
		int32_t neededBytes = ((m_BitPosition + amount + 7) >> 3);

		if (neededBytes > static_cast<int32_t>(m_Buffer.size())) {
			m_Buffer.resize(neededBytes + DEFAULT_BUFFER_SIZE); // Grow in chunks
		}

		int32_t bitOffset = 8 - (m_BitPosition & 7);
		m_BitPosition += amount;

		for (; amount > bitOffset; bitOffset = 8) {
			m_Buffer[bytePos] &= ~BIT_MASK[bitOffset];
			m_Buffer[bytePos++] |= (value >> (amount - bitOffset)) & BIT_MASK[bitOffset];
			amount -= bitOffset;
		}

		if (amount == bitOffset) {
			m_Buffer[bytePos] &= ~BIT_MASK[bitOffset];
			m_Buffer[bytePos] |= value & BIT_MASK[bitOffset];
		}
		else {
			m_Buffer[bytePos] &= ~(BIT_MASK[amount] << (bitOffset - amount));
			m_Buffer[bytePos] |= (value & BIT_MASK[amount]) << (bitOffset - amount);
		}
	}
}
