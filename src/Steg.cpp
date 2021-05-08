#include "Steg.h"
#include "image/Image.h"
#include "StegUtils.h"
#include "lzo/LZOCompression.h"
#include <iostream>
#include <cmath>
#include "BitIO.h"
#include <cstring>

void Steg::Embed(std::vector<uint8_t>& cover, std::vector<uint8_t>& data, const DataHeader& header, bool* error) {
	// Check that the data + header will actually fit into cover
	if((ceil((float)data.size() / (float)header.bitsPerByte) + sizeof(DataHeader) * 8) > cover.size()) {
		std::cout << "[ERROR]: Cannot fit data into cover file" << std::endl;
		if(error) {
			*error = true;
		}
		return;
	}

	// Allocate space at the start of the vector for the header
	data.insert(data.begin(), sizeof(DataHeader), 0);

	// Copy header into that space
	memcpy(data.data(), (void*)&header, sizeof(DataHeader));

	// Reading from data (now including the header at the front)
	BitReader reader(data.data(), data.size() * 8ul);

	// Writing to cover
	BitWriter writer(cover.data(), cover.size() * 8ul, 1, true);

	// Use the BitWriter and BitReader to read from data and write to cover
	while(!(reader.Done() || writer.Done())) {
		writer.WriteNextBit(reader.ReadNextBit());
		if(writer.m_OverallBitIndex == (sizeof(DataHeader) * 8ul)) {
			writer.m_BitsPerByte = header.bitsPerByte;
		}
	}
}

std::vector<uint8_t> Steg::Extract(std::vector<uint8_t>& stegoData, DataHeader* header, bool* error) { // Huh, error is unused. It's not obvious though in what way this can error without segfaulting
	// Buffer for holding extracted data
	std::vector<uint8_t> data;

	// Resize to be able to hold the header
	data.resize(sizeof(DataHeader));

	// Reading from stegoData
	BitReader reader(stegoData.data(), sizeof(DataHeader) * 8ul, 1, true);

	// Writing to data
	BitWriter writer(data.data(), sizeof(DataHeader) * 8ul);

	// Read the header into data
	while(!(reader.Done() || writer.Done())) {
		writer.WriteNextBit(reader.ReadNextBit());
	}

	DataHeader hdr;

	// TODO The way I've got this means that if the header was embedded using different endianness to when extracting, then things will go wrong and bad. Which isn't good, change this
	// Copy the just read header into the header object.
	memcpy((void*)&hdr, data.data(), sizeof(DataHeader));

	// If the data hasn't been compressed then the compressed size should be the same as the decompressed size in the header
	uint32_t dataSize = hdr.compressedSize;

	// Make space for the actual payload I guess you could call it
	data.resize(dataSize);

	// Not recreating object to keep its offset. Which I could just set but ehhh
	reader.m_NumBits += dataSize * 8ul;
	reader.m_Done = false;
	reader.m_BitsPerByte = hdr.bitsPerByte;

	writer = BitWriter(data.data(), data.size() * 8ul);

	// Read the rest of the data into data from stegoData
	while(!(reader.Done() || writer.Done())) {
		writer.WriteNextBit(reader.ReadNextBit());
	}

	if(header) {
		*header = hdr;
	}

	return data;
}
