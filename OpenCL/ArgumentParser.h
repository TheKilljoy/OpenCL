#pragma once

struct Options;

class ArgumentParser
{
	// minimum arguments:
	// 0: path to .exe
	// 1: input filepath
	// 3: mode (gaussianBlur or YCbCr)
private:
	const int minArgs = 3;
public:
	bool parseArguments(const int argc, char** argv, Options& options);
private:
	void printUsage(int error);
	void putFilepathAndFilenameIntoOptions(const std::string& filepath, Options& options);
	void extractFilesFromFolder(const std::string& path, Options& options);
};

