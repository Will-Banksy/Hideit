#include "parser/Global.h"
#include "parser/CommandLineParser.h"

// TODO Turn the Command Line Parser stuff into like a proper library

int main(int argc, char** argv) {
	CommandLineParser parser("SteganographyExe", "1.0");

	String inputFile;
	String outputFile;
	String dataFile;
	bool showHelp = false;
	bool showVersion = false;

	parser.AddArgs({ new CommandLineArg("input", "the input file", &inputFile), new CommandLineArg("output", "the output file", &outputFile) });
	parser.AddOptions({
		new CommandLineOption({"-h", "--help"}, "Displays this help", &showHelp),
		new CommandLineOption({"--version"}, "Displays the program version", &showVersion),
		new CommandLineOption({"-df", "--datafile"}, "Allows you to specify the datafile", nullptr, { new CommandLineArg("datafile", "the file that contains the data to be hidden", &dataFile) })
	});

	parser.Parse(argc, argv);

	if(showHelp) {
		parser.Help();
		return 0;
	}

	if(showVersion) {
		parser.Version();
		return 0;
	}

	std::cout << "inputFile: " << inputFile << ", outputFile: " << outputFile << ", dataFile: " << dataFile << ", showHelp: " << showHelp << ", showVersion: " << showVersion << std::endl;

	return 0;
}