#ifndef STEG_H
#define STEG_H

#include <string>
#include <vector>

class Image;

// The pragma pack will make sure that this struct won't do any byte alignment: See https://stackoverflow.com/questions/50005694/reading-binary-file-into-a-struct-using-c-cli
#pragma pack(push, 1)
struct DataHeader {
	bool compressed;
	bool encrypted;
	uint8_t bitsPerByte;
	uint32_t decompressedSize;
	uint32_t compressedSize;

	DataHeader() : compressed(false), encrypted(0), bitsPerByte(0), decompressedSize(0), compressedSize(0) {}
	DataHeader(bool compressed, bool encrypted, uint8_t bitsPerByte, uint32_t decompressedSize, uint32_t compressedSize) : compressed(compressed), encrypted(encrypted), bitsPerByte(bitsPerByte), decompressedSize(decompressedSize), compressedSize(compressedSize) {}
};
#pragma pack(pop)

class Steg {
public:
	/// Embeds <code>data</code> into <code>cover</code> writing <code>bitsPerByte</code> bits into each byte of <code>cover</code>, writing the result into <code>cover</code><br>
	/// If an error occurs, <code>error</code> will be 1, otherwise it'll be 0<br>
	/// The <code>header</code> provides options to the function such as <code>bitsPerByte</code>, and is embedded to provide information necessary to extraction
	/// <code>data</code> is not marked const because it gets modified in the function but is returned in the same state it was inputted. Doing this to avoid making a whole new buffer
	static void Embed(std::vector<uint8_t>& cover, std::vector<uint8_t>& data, const DataHeader& header, bool* error = nullptr);
	/// Extracts and returns embedded data in <code>stegoData</code>. If an error occurs, <code>error</code> will be 1, otherwise it'll be 0<br>
	/// The header is extracted from the <code>stegoData</code> and written into <code>header</code> if not null
	static std::vector<uint8_t> Extract(std::vector<uint8_t>& stegoData, DataHeader* header = nullptr, bool* error = nullptr);
};

#endif // STEG_H
