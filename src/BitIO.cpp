#include "BitIO.h"

BitReader::BitReader(const uint8_t* bytes, uint64_t numBits, uint8_t bitsPerByte, bool startAtLSB) : m_Bytes(bytes), m_NumBits(numBits),
																								m_BitsPerByte(bitsPerByte), m_StartAtLSB(startAtLSB),
																								m_Done(false), m_ByteIndex(0), m_BitIndex(0), m_OverallBitIndex(0) {
}

bool BitReader::ReadNextBit() {
	uint8_t byte = m_Bytes[m_ByteIndex];
	uint8_t bitIndex = m_StartAtLSB ? m_BitIndex : 7 - m_BitIndex;

	bool value = byte & (0x1 << bitIndex);

	m_BitIndex++;
	if(m_BitIndex >= m_BitsPerByte) {
		m_BitIndex = 0;
		m_ByteIndex++;
	}
	m_OverallBitIndex++;
	if(m_OverallBitIndex >= m_NumBits) {
		m_Done = true;
	}

	return value;
}

BitWriter::BitWriter(uint8_t* bytes, uint64_t numBits, uint8_t bitsPerByte, bool startAtLSB) : m_Bytes(bytes), m_NumBits(numBits),
																								m_BitsPerByte(bitsPerByte), m_StartAtLSB(startAtLSB),
																								m_Done(false), m_ByteIndex(0), m_BitIndex(0), m_OverallBitIndex(0) {
}

void BitWriter::WriteNextBit(bool value) {
	uint8_t& byte = m_Bytes[m_ByteIndex];
	uint8_t bitIndex = m_StartAtLSB ? m_BitIndex : 7 - m_BitIndex;

	if(value) {
		byte = byte | (0x1 << bitIndex);
	} else {
		byte = byte & (~(0x1 << bitIndex));
	}

	m_BitIndex++;
	if(m_BitIndex >= m_BitsPerByte) {
		m_BitIndex = 0;
		m_ByteIndex++;
	}
	m_OverallBitIndex++;
	if(m_OverallBitIndex >= m_NumBits) {
		m_Done = true;
	}
}
