#include "Steg.h"
#include "image/Image.h"
#include "StegUtils.h"
#include "lzo/LZOCompression.h"
#include <iostream>
#include <cmath>
#include "BitIO.h"
#include <cstring>

void Steg::EmbedInImage(Image& coverImage, std::vector<uint8_t>& data, uint8_t bitsPerChannel, bool* error) {
	if(coverImage.m_Format == Format_Invalid) {
		std::cout << "[ERROR]: Image format invalid" << std::endl;
		if(error) {
			*error = true;
		}
		return;
	}

	uint64_t numBytes = coverImage.m_Width * coverImage.m_Height * coverImage.m_Format;

	if(data.size() > 0xffffffff) {
		std::cout << "[ERROR]: Too much data - Supported max amount of data: 0xffffffff (4294967295) bytes" << std::endl;
		if(error) {
			*error = true;
		}
		return;
	}

	// TODO Maybe have the compression as a separate stage to embedding?
	bool compError = false;
	bool compressed = true;
	std::vector<uint8_t> dataComp;
	LZOCompression::Compress(data, dataComp, &compError);
	std::vector<uint8_t>* dataCompPtr = &dataComp;

	if(compError) {
		std::cout << "[WARN]: LZO compression failed - Continuing with uncompressed data" << std::endl;
		dataCompPtr = &data;
		compressed = false;
	}

	if(dataCompPtr->size() >= data.size()) {
		std::cout << "[WARN]: Uncompressible data - Continuing with uncompressed data" << std::endl;
		dataCompPtr = &data;
		compressed = false;
	}

	DataHeader header = { compressed, bitsPerChannel, (uint32_t)data.size(), (uint32_t)dataCompPtr->size() };
	if(header.decompressedSize != data.size() || header.compressedSize != dataCompPtr->size()) {
		std::cout << "[ERROR]: Big bad error - data too big boohoo" << std::endl;
		if(error) {
			*error = true;
		}
		return;
	}

	if((ceil((float)dataCompPtr->size() / (float)bitsPerChannel) + sizeof(DataHeader) * 8) > numBytes) {
		std::cout << "[ERROR]: Cannot fit data into cover file" << std::endl;
		if(error) {
			*error = true;
		}
		return;
	}

	// Allocate space at the start of the array for the header
	dataCompPtr->insert(dataCompPtr->begin(), sizeof(DataHeader), 0);

	// Copy the header into that space
	memcpy(dataCompPtr->data(), (void*)&header, sizeof(DataHeader));

	BitReader reader(dataCompPtr->data(), dataCompPtr->size() * 8ul);

	BitWriter writer(coverImage.m_Bytes, numBytes * 8ul, 1, true);

	while(!(reader.Done() || writer.Done())) {
		writer.WriteNextBit(reader.ReadNextBit());
		if(writer.m_OverallBitIndex == (sizeof(DataHeader) * 8ul)) {
			writer.m_BitsPerByte = bitsPerChannel;
		}
	}

// 	std::cout << "header.compressed: " << header.compressed << std::endl;
// 	std::cout << "header.bitsPerChannel: " << (uint16_t)header.bitsPerChannel << std::endl;
// 	std::cout << "header.decompressedSize: " << header.decompressedSize << std::endl;
// 	std::cout << "header.compressedSize: " << header.compressedSize << std::endl;
}

std::vector<uint8_t> Steg::ExtractFromImage(Image& stegoImage, bool* error) {
	uint64_t numBytes = stegoImage.m_Width * stegoImage.m_Height * stegoImage.m_Format;

	std::vector<uint8_t> data;
	data.resize(sizeof(DataHeader));

	BitReader reader(stegoImage.m_Bytes, sizeof(DataHeader) * 8ul, 1, true);

	BitWriter writer(data.data(), sizeof(DataHeader) * 8ul);

	while(!(reader.Done() || writer.Done())) {
		writer.WriteNextBit(reader.ReadNextBit());
	}

	DataHeader header {0, 0, 0};

	// Copy everything from data to the header
	memcpy((void*)&header, data.data(), sizeof(DataHeader));

// 	std::cout << "header.compressed: " << header.compressed << std::endl;
// 	std::cout << "header.bitsPerChannel: " << (uint16_t)header.bitsPerChannel << std::endl;
// 	std::cout << "header.decompressedSize: " << header.decompressedSize << std::endl;
// 	std::cout << "header.compressedSize: " << header.compressedSize << std::endl;

	reader.m_NumBits += numBytes * 8ul;
	reader.m_Done = false;
	reader.m_BitsPerByte = header.bitsPerChannel;

	uint32_t dataSize = header.compressed ? header.compressedSize : header.decompressedSize;

	data.resize(dataSize);

	writer = BitWriter(data.data(), dataSize * 8ul);

	while(!(reader.Done() || writer.Done())) {
		writer.WriteNextBit(reader.ReadNextBit());
	}

	if(header.compressed) {
		std::vector<uint8_t> decompressedData;
		LZOCompression::Decompress(data, decompressedData, header.decompressedSize);
		return decompressedData;
	}

	if(error) {
		*error = false;
	}
	return data;
}
