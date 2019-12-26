#pragma once
#include <vector>
#include <string>

struct Options
{
	bool gaussianBlur = false;
	bool conversionRGBToYCbCr = false;
	std::vector<std::string> inputFiles;
	std::vector<std::string> fileNames;
	std::string outputDir = "Output";
	std::string currentFilePath;
	float sigma;
};
