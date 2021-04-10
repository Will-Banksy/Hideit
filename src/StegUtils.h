#ifndef STEGUTILS_H
#define STEGUTILS_H

#include <cstdint>
#include <vector>
#include <string>

class StegUtils {
public:
	static std::vector<uint8_t> ReadBinaryFile(const std::string& filename);
	static void WriteBinaryFile(const std::string& filename, const std::vector<uint8_t>& data);

	static bool GetBitAt(uint8_t byte, uint8_t bitIndex);
	static void SetBitAt(uint8_t& byte, uint8_t bitIndex, bool value);

	static uint8_t ParseUint8(const std::string& str);
};

#endif // STEGUTILS_H
