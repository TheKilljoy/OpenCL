#include "pch.h"
#include "ArgumentParser.h"
#include "Options.h"

struct ArgumentAndParameters
{
	std::string argument;
	std::vector<std::string> parameters;
};

bool ArgumentParser::parseArguments(const int argc, char** argv, Options& options)
{
	if (argc < minArgs)
	{
		std::cout << "Usage:\n"\
			<< "-if: path to input file(s), sperate them with a space\n"\
			<< "-od: path to output directory\n"\
			<< "-gb: use gaussian blur on input files\n"\
			<< "-cv: use conversion from rgb to ycbcr on input files\n\n"\
			<< "Specify at least one input file, and a mode (gaussian blur or conversion)\n";
		std::exit(-1);
	}
	std::vector<std::unique_ptr<ArgumentAndParameters>> argumentsAndParameters;
	std::ifstream fileChecker;

	options.currentFilePath = argv[0];
	options.currentFilePath = options.currentFilePath.substr(0, options.currentFilePath.find_last_of('.'));
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
		if (argAndPara->argument.compare("-if") == 0)
		{
			if (argAndPara->parameters.size() == 0) return false;
			for (const auto& parameter : argAndPara->parameters)
			{
				options.inputFiles.push_back(parameter);
				if (parameter.find('\\') != std::string::npos)
				{
					options.fileNames.push_back(parameter.substr(
						parameter.find_last_of('\\'), parameter.length())
					);
				}
				else
				{
					options.fileNames.push_back(parameter.substr(
						0, parameter.length())
					);
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
			options.gaussianBlur = true;
		}

		if (argAndPara->argument.compare("-cv") == 0)
		{
			options.conversionRGBToYCbCr = true;
		}
	}

	return true;
}
