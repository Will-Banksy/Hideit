#ifndef STEG_H
#define STEG_H

#include <string>
#include <vector>

class Image;

// The pragma pack will make sure that this struct won't do any byte alignment: See https://stackoverflow.com/questions/50005694/reading-binary-file-into-a-struct-using-c-cli
#pragma pack(push, 1)
struct DataHeader {
	bool compressed;
	uint8_t bitsPerChannel;
	uint32_t decompressedSize;
	uint32_t compressedSize;
};
#pragma pack(pop)

class Steg {
public:
	/// <code>bitsPerChannel</code> is the number of bits to hide in a single channel (a channel being like a single component (red, green or blue) of RGB) (inserts a header at the front that always uses 1 bit per channel)
	static void EmbedInImage(Image& coverImage, std::vector<uint8_t>& data, uint8_t bitsPerChannel = 2, bool* error = nullptr);
	static std::vector<uint8_t> ExtractFromImage(Image& stegoImage, bool* error = nullptr);
};

#endif // STEG_H
