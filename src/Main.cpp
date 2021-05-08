#include "parser/CommandLineParser.h"
#include "Steg.h"
#include "StegUtils.h"
#include "image/Image.h"
#include <sstream>
#include "BitIO.h"
#include <cctype>
#include "lzo/LZOCompression.h"
#include "aes/AESEncryption.h"

// TODO Make the stego stuff into a library. Eventually
// TODO Needs lots of error checking. For example, need to check if files exist before opening them!
// Currently if *anything* goes wrong, it just continues as if nothing has gone wrong until it segfaults. Desperately needs error checking

int main(int argc, char** argv) {
	CommandLineParser parser("hideit", "1.0");

	std::string coverFile;
	std::string stegoFile;
	std::string dataFile;
	bool showHelp = false;
	bool showVersion = false;
	std::string operation;
	std::string bpbStr;
	bool compress = false;
	std::string passkeyStr;

	parser.AddArgs({ new CommandLineArg("operation", "The operation to perform, must be either embed or extract\n\t\tembed\t- Embed dataFile in coverFile, saving output as stegoFile\n\t\textract\t- Extract data from stegoFile, saving output as dataFile", &operation) });
	parser.AddOptions({
		new CommandLineOption({ "-h", "--help" }, "Displays this help", &showHelp),
		new CommandLineOption({ "--version" }, "Displays the program version", &showVersion),
		new CommandLineOption({ "-df", "--datafile" }, "Allows you to specify the datafile - If not specified, will take data from stdin (for embedding) and output to stdout (for extracting)", nullptr, { new CommandLineArg("datafile", "the file that contains the data to be hidden", &dataFile) }),
		new CommandLineOption({ "-sf", "--stegofile" }, "Allows you to specify the stegofile", nullptr, { new CommandLineArg("stegofile", "the file that contains the hidden data", &stegoFile) }),
		new CommandLineOption({ "-cf", "--coverfile" }, "Allows you to specify the coverfile", nullptr, { new CommandLineArg("coverfile", "the file that is used to hide the data", &coverFile) }),
		new CommandLineOption({ "-bpb", "--bits-per-byte" }, "Allows you to specify the number of bits of data stored in each byte (1-8) - More bpb is more noticeable", nullptr, { new CommandLineArg("bpb", "unseen help", &bpbStr) }),
		new CommandLineOption({ "-c", "--compress" }, "If present, the data is compressed before being embedded" , &compress),
		new CommandLineOption({ "-p", "--passkey" }, "If present, AES-128 encryption is used to encrypt and decrypt the data with the given passkey", nullptr, { new CommandLineArg("passkey", "unseen help", &passkeyStr) })
	});

	parser.Parse(argc, argv);

	if(showHelp) {
		if(showVersion) {
			parser.Version();
		}
		parser.Help();
		return 0;
	}

	if(showVersion) {
		parser.Version();
		return 0;
	}

	// TODO Error checking and handling
	if(operation == "embed") {
		if(coverFile != "" && stegoFile != "") {
			std::vector<uint8_t> data;
			if(dataFile == "") {
				std::cout << "Data file not specified - taking from stdin until EOF (Windows: Ctrl+Z, Unix: Ctrl+D)" << std::endl;
				// Now we get input until EOF - On Windows typically you'd enter that with Ctrl+Z, on Unix it's typically Ctrl+D
				while(!std::cin.eof()) {
					std::string line;
					getline(std::cin, line);
					if(!std::cin.eof()) {
						line.append("\n");
					}
					data.insert(data.end(), line.begin(), line.end());
				}
			} else {
				data = StegUtils::ReadBinaryFile(dataFile);
			}
			uint8_t bpb = 1;
			if(bpbStr != "") {
				bpb = StegUtils::ParseUint8(bpbStr);
				if(bpb == 0) bpb = 1;
				if(bpb > 8) bpb = 8;
			}

			// Load the image, put the data into a std::vector, delete the old image data and make the coverImage.m_Bytes point to the vector data
			Image coverImage(coverFile);
			std::vector<uint8_t> coverData = StegUtils::CreateFrom(coverImage.m_Bytes, coverImage.m_Width * coverImage.m_Height * coverImage.m_Format);
			delete[] coverImage.m_Bytes; // Delete the old data
			coverImage.m_Bytes = coverData.data(); // The data in coverData is the data we're working on

			uint32_t uncompressedSize = data.size();
			if(compress) {
				std::vector<uint8_t> dataComp; // Compressed data
				LZOCompression::Compress(data, dataComp);
				data = dataComp;
			}

			bool encrypted = !passkeyStr.empty();

			if(encrypted) {
				std::vector<uint8_t> dataEnc; // Encrypted data
				std::vector<uint8_t> passkey;
				passkey.resize(passkeyStr.size());
				memcpy(passkey.data(), passkeyStr.data(), passkeyStr.size()); // Copy passkeyStr into passkey
				passkeyStr.clear(); // Delete the passkey string data
				dataEnc = AESEncryption::AES128(data, passkey);
				data = dataEnc;
			}

			DataHeader header(compress, encrypted, bpb, uncompressedSize, data.size());

			// Embed the data into coverData. This will change coverData
			Steg::Embed(coverData, data, header, nullptr);

			// Convert file extension to lowercase - Should probably add support for uppercase file extensions in this and ilib but this will do for now
			std::string::iterator startFrom = stegoFile.begin() + stegoFile.find_last_of('.');
			std::transform(startFrom, stegoFile.end(), startFrom, tolower);

			auto stegoFilenameParts = StegUtils::Split(stegoFile, '.');
			std::string& extension = stegoFilenameParts.back();

			ImageFileType format = FileType_Auto;
			if(extension == "jpg") { // JPG is unsupported - it uses lossy compression which corrupts any data we try hide in it
				format = FileType_PNG;
			}
			if(extension != "png" && extension != "tga" && extension != "bmp") {
				std::cout << "[WARN]: Unsupported image format - Saving as png" << std::endl;
			}

			coverImage.Save(stegoFile, format);

			coverImage.m_Bytes = new uint8_t(); // Do this because currently m_Bytes is pointing to data contained in a vector, the vector will delete the data and the destructor of Image will try to do the same, or vice-versa, which is bad
		} else {
			std::vector<std::string> missing;
			if(coverFile == "") {
				missing.push_back("coverFile");
			}
			if(stegoFile == "") {
				missing.push_back("stegoFile");
			}
			std::stringstream s;
			for(int i = 0; i < missing.size(); i++) {
				if(i > 0) {
					s << ", ";
				}
				s << missing.at(i);
			}
			s.flush();
			std::cout << "[ERROR]: " << s.str() << " undefined" << std::endl;
		}
	} else if(operation == "extract") {
		if(stegoFile != "") {
			Image stegoImage(stegoFile);
			std::vector<uint8_t> stegoData = StegUtils::CreateFrom(stegoImage.m_Bytes, stegoImage.m_Width * stegoImage.m_Height * stegoImage.m_Format);
			stegoImage.Free(); // Don't need the image object any more, so we just free it's memory

			DataHeader header;
			std::vector<uint8_t> data = Steg::Extract(stegoData, &header);

			if(header.encrypted) {
				std::vector<uint8_t> dataEnc; // Encrypted data
				std::vector<uint8_t> passkey;
				passkey.resize(passkeyStr.size());
				memcpy(passkey.data(), passkeyStr.data(), passkeyStr.size()); // Copy passkeyStr into passkey
				passkeyStr.clear(); // Delete the passkey string data
				dataEnc = AESEncryption::AES128(data, passkey);
				data = dataEnc;
			}

			if(header.compressed) {
				std::vector<uint8_t> dataDecomp;
				LZOCompression::Decompress(data, dataDecomp, header.decompressedSize);
				data = dataDecomp;
			}

			if(dataFile == "") {
				for(uint8_t ch : data) {
					std::cout << ch;
				}
			} else {
				StegUtils::WriteBinaryFile(dataFile, data);
			}
		} else {
			std::vector<std::string> missing;
			if(dataFile == "") {
				missing.push_back("dataFile");
			}
			if(stegoFile == "") {
				missing.push_back("stegoFile");
			}
			std::stringstream s;
			for(int i = 0; i < missing.size(); i++) {
				if(i > 0) {
					s << ", ";
				}
				s << missing.at(i);
			}
			s.flush();
			std::cout << "[ERROR]: " << s.str() << " undefined" << std::endl;
		}
	} else if(operation == "") {
		parser.Help();
	} else {
		std::cout << "[ERROR]: Unsupported operation" << std::endl;
	}

	return 0;
}