#include "StegUtils.h"
#include <fstream>
#include <vector>
#include <sstream>

std::vector<uint8_t> StegUtils::ReadBinaryFile(const std::string& filename) {
	std::ifstream filestream(filename, std::ios::binary);
	std::vector<uint8_t> data = std::vector<uint8_t>();
	filestream.seekg(0, filestream.end);
	auto length = filestream.tellg();
	data.resize(length);
	filestream.seekg(0, filestream.beg);
	filestream.read((char*)&data.front(), length);
	return data;
}

void StegUtils::WriteBinaryFile(const std::string& filename, const std::vector<uint8_t>& data) {
	std::ofstream filestream(filename, std::ios::binary);
	filestream.write((char*)&data.front(), data.size());
}

bool StegUtils::GetBitAt(uint8_t byte, uint8_t bitIndex) {
	if(bitIndex > 7) {
		return 0;
	}
	bitIndex = 7 - bitIndex;
	return byte >> bitIndex & 0x1;
}

void StegUtils::SetBitAt(uint8_t& byte, uint8_t bitIndex, bool value) {
	if(bitIndex > 7) {
		return;
	}
	bitIndex = 7 - bitIndex;
	uint8_t mask = 0x1 << bitIndex;
	if(value) {
		byte = byte | mask;
	} else {
		byte = byte & (~mask);
	}
}

uint8_t StegUtils::ParseUint8(const std::string& str) {
	unsigned long lresult = stoul(str, 0, 10);
	uint8_t result = lresult;
	if (result != lresult) throw std::out_of_range("Out of range");
	return result;
}

std::vector<std::string> StegUtils::Split(std::string str, char delim, bool ignoreEmpty) {
	// Current token
	std::stringstream token;
	std::vector<std::string> tokens;

	for(int i = 0; i < str.length(); i++) {
		if(str.at(i) == delim) {
			token.flush();
			if(!ignoreEmpty || !token.str().empty()) {
				tokens.push_back(token.str());
			}
			token.str(std::string()); // Empty the current token
		} else {
			token << char(str.at(i)); // Should probably make sure it's copied
		}
	}
	token.flush();
	if(!ignoreEmpty || !token.str().empty()) {
		tokens.push_back(token.str());
	}

	return tokens;
}