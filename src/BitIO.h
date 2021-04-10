#ifndef BITIO_H
#define BITIO_H

#include <cstdint>

struct BitReader {
	uint8_t* m_Bytes;
	uint64_t m_NumBits;
	uint8_t m_BitsPerByte;
	bool m_StartAtLSB;
	bool m_Done;

	BitReader(uint8_t* bytes, uint64_t numBits, uint8_t bitsPerByte = 8, bool startAtLSB = false);

	bool ReadNextBit();
	bool Done() { return m_Done; }

// private:
	uint32_t m_ByteIndex;
	uint8_t m_BitIndex;
	uint64_t m_OverallBitIndex;
};

struct BitWriter {
	uint8_t* m_Bytes;
	uint64_t m_NumBits;
	uint8_t m_BitsPerByte;
	bool m_StartAtLSB;
	bool m_Done;

	BitWriter(uint8_t* bytes, uint64_t numBits, uint8_t bitsPerByte = 8, bool startAtLSB = false);

	void WriteNextBit(bool value);
	bool Done() { return m_Done; }

// private:
	uint32_t m_ByteIndex;
	uint8_t m_BitIndex;
	uint64_t m_OverallBitIndex;
};

#endif // BITIO_H
