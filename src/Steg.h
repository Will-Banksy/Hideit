#ifndef STEG_H
#define STEG_H

#include <string>

class Steg {
public:
	static void HideInImage(std::string coverImageFile, std::string dataFile, std::string outputFile);
};

#endif // STEG_H
