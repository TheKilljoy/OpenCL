#include "pch.h"
#include "ArgumentParser.h"
#include "Options.h"
#include <filesystem>

struct ArgumentAndParameters
{
	std::string argument;
	std::vector<std::string> parameters;
};

bool ArgumentParser::parseArguments(const int argc, char** argv, Options& options)
{
	if (argc < minArgs)
	{
		printUsage(-1);
	}
	std::vector<std::unique_ptr<ArgumentAndParameters>> argumentsAndParameters;
	std::ifstream fileChecker;

	options.currentFilePath = argv[0];
	options.currentFilePath = options.currentFilePath.substr(0, options.currentFilePath.find_last_of('\\'));
	options.outputDir = options.currentFilePath + "\\Output";
	for (int i = 1; i < argc; ++i)
	{
		if (argv[i][0] == '-')
		{
			argumentsAndParameters.push_back(std::make_unique<ArgumentAndParameters>());
			argumentsAndParameters.back()->argument = argv[i];
		}
		else
		{
			argumentsAndParameters.back()->parameters.push_back(argv[i]);
		}
	}

	for (const auto& argAndPara : argumentsAndParameters)
	{
		if (argAndPara->argument.compare("-h") == 0 || argAndPara->argument.compare("-help") == 0)
		{
			printUsage(0);
		}

		if (argAndPara->argument.compare("-if") == 0)
		{
			if (argAndPara->parameters.size() == 0) return false;
			for (const auto& parameter : argAndPara->parameters)
			{
				if (parameter.find_last_of('.') != std::string::npos)
				{
					std::string extension = parameter.substr(parameter.find_last_of('.'), 4);
					if (extension.compare(".png") == 0 ||
						extension.compare(".jpg") == 0)
					{
						putFilepathAndFilenameIntoOptions(parameter, options);
					}
				}
				else
				{
					extractFilesFromFolder(parameter, options);
				}
			}
		}

		if (argAndPara->argument.compare("-od") == 0)
		{
			if (argAndPara->parameters.size() != 1) return false;
			options.outputDir = argAndPara->parameters[0];
		}

		if (argAndPara->argument.compare("-gb") == 0)
		{
			if (argAndPara->parameters.size() == 0)
			{
				std::cout << "Gaussian blur needs a value for sigma as parameter\n";
				return false;
			}
			options.sigma = std::stof(argAndPara->parameters[0]);
			options.gaussianBlur = true;
		}

		if (argAndPara->argument.compare("-cv") == 0)
		{
			options.conversionRGBToYCbCr = true;
		}
	}

	if (!std::filesystem::exists(options.outputDir))
		std::filesystem::create_directory(options.outputDir);

	return true;
}

void ArgumentParser::printUsage(int error)
{
	std::cout << "Usage:\n"\
		<< "-if: path to input file(s) or input folder(s), sperate them with a space\n"\
		<< "-od: path to output directory\n"\
		<< "-gb: <sigma> use gaussian blur on input files\n"\
		<< "-cv: use conversion from rgb to ycbcr on input files\n\n"\
		<< "Specify at least one input file/folder, and a mode (gaussian blur or conversion)\n";
	std::exit(error);
}

void ArgumentParser::putFilepathAndFilenameIntoOptions(const std::string& filepath, Options& options)
{
	options.inputFiles.push_back(filepath);
	if (filepath.find('\\') != std::string::npos)
	{
		options.fileNames.push_back(filepath.substr(
			filepath.find_last_of('\\') + 1, filepath.length())
		);
	}
	else
	{
		options.fileNames.push_back(filepath.substr(
			0, filepath.length())
		);
	}
}

void ArgumentParser::extractFilesFromFolder(const std::string& path, Options& options)
{
	std::filesystem::path dirWithImages = options.currentFilePath;
	dirWithImages.append(path);
	for (const auto& file : std::filesystem::directory_iterator(dirWithImages))
	{
		std::filesystem::path p = file.path();
		if (p.extension().compare(".jpg") == 0 ||
			p.extension().compare(".png") == 0)
		{
			putFilepathAndFilenameIntoOptions(p.string(), options);
		}
		if (!p.has_extension())
		{
			extractFilesFromFolder(p.string(), options);
		}
	}
}
