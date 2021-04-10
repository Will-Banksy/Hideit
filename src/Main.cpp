#include "parser/CommandLineParser.h"
#include "Steg.h"
#include "StegUtils.h"
#include "image/Image.h"
#include <sstream>
#include "BitIO.h"

// TODO Make the stego stuff into a library. Eventually
// TODO Add AES encryption capabilities

int main(int argc, char** argv) {
	CommandLineParser parser("SteganographyExe", "1.0");

	std::string coverFile;
	std::string stegoFile;
	std::string dataFile;
	bool showHelp = false;
	bool showVersion = false;
	std::string operation;
	std::string bpbStr;

	parser.AddArgs({ new CommandLineArg("operation", "The operation to perform, must be either embed or extract", &operation) });
	parser.AddOptions({
		new CommandLineOption({ "-h", "--help" }, "Displays this help", &showHelp),
		new CommandLineOption({ "--version" }, "Displays the program version", &showVersion),
		new CommandLineOption({ "-df", "--datafile" }, "Allows you to specify the datafile - If not specified, will take data from stdin (for embedding) and output to stdout (for extracting)", nullptr, { new CommandLineArg("datafile", "the file that contains the data to be hidden", &dataFile) }),
		new CommandLineOption({ "-sf", "--stegofile" }, "Allows you to specify the stegofile", nullptr, { new CommandLineArg("stegofile", "the file that contains the hidden data", &stegoFile) }),
		new CommandLineOption({ "-cf", "--coverfile" }, "Allows you to specify the coverfile", nullptr, { new CommandLineArg("coverfile", "the file that is used to hide the data", &coverFile) }),
		new CommandLineOption({ "-bpb", "--bits-per-byte" }, "Allows you to specify the number of bits of data stored in each byte (1-8) - More bpb is more noticeable", nullptr, { new CommandLineArg("bpb", "unseen help", &bpbStr) })
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

// 	std::vector<uint8_t> arr = { 0b00000010, 0b00000010, 0b00000010, 0b00000010, 0b00000010, 0b00000010, 0b00000010, 0b00000010 };
// 	BitReader reader(arr.data(), 12, 1, true);
// 	while(!reader.Done()) {
// 		std::cout << reader.ReadNextBit();
// 		if(reader.m_OverallBitIndex == 4) {
// 			reader.m_BitsPerByte = 2;
// 			std::cout << "|";
// 		}
// 	}
// 	std::cout << std::endl;
//
// 	return 0;

	if(operation == "embed") {
		if(coverFile != "" && stegoFile != "") {
			std::vector<uint8_t> data;
			if(dataFile == "") {
				std::cout << "Data file not specified - taking from stdin until EOF (Windows: Ctrl+Z, Unix: Ctrl+D)" << std::endl;
				// Now we get input until EOF - On Windows typically you'd enter that with Ctrl+Z, on Unix it's typically Ctrl+D
				while(!std::cin.eof()) {
					std::string line;
					getline(std::cin, line);
					line.append("\n");
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
			Image coverImage(coverFile);
			Steg::EmbedInImage(coverImage, data, bpb);
			coverImage.Save(stegoFile);
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
			std::vector<uint8_t> data = Steg::ExtractFromImage(stegoImage);
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