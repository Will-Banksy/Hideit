#include "parser/CommandLineParser.h"
#include "Steg.h"

// TODO Make the stego stuff into a library. Eventually

int main(int argc, char** argv) {
	CommandLineParser parser("SteganographyExe", "1.0");

	std::string coverFile;
	std::string stegoFile;
	std::string dataFile;
	bool showHelp = false;
	bool showVersion = false;

	parser.AddArgs({ new CommandLineArg("operation", "The operation to perform, must be either embed or extract") });
	parser.AddOptions({
		new CommandLineOption({"-h", "--help"}, "Displays this help", &showHelp),
		new CommandLineOption({"--version"}, "Displays the program version", &showVersion),
		new CommandLineOption({"-df", "--datafile"}, "Allows you to specify the datafile - If not specified, will take data from stdin", nullptr, { new CommandLineArg("datafile", "the file that contains the data to be hidden", &dataFile) }),
		new CommandLineOption({"-sf", "--stegofile"}, "Allows you to specify the stegofile", nullptr, { new CommandLineArg("stegofile", "the file that contains the hidden data", &stegoFile) }),
		new CommandLineOption({"-cf", "--coverfile"}, "Allows you to specify the coverfile", nullptr, { new CommandLineArg("coverfile", "the file that is used to hide the data", &coverFile) })
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

	return 0;
}