#include "pch.h"
#include "OpenCLProgram.h"
#include "CL/cl.hpp"
#include "Timer.h"
#include "Mask.h"
#include "Options.h"
#include "ArgumentParser.h"
#include "opencv2/opencv.hpp"
#include "ImageProcess.h"
#include <filesystem>
#include "FileWriter.h"



int main(int argc, char** argv)
{
	ArgumentParser parser;
	Options options;
	// check arguments
	if (!parser.parseArguments(argc, argv, options))
	{
		std::cout << "Error while parsing command line arguments\n";
		std::exit(1);
	}
  
	ImageProcess process;
	std::vector<cv::Mat> originals;
	std::vector<cv::Mat> resultsGPUGuassianBlur;
	std::vector<cv::Mat> resultsCPUGaussianBlur;
	std::vector<cv::Mat> resultsGPUYCbCr;
	std::vector<cv::Mat> resultsCPUYCbCr;
	std::vector<cv::Mat> deltas;
	Timer t;
	FileWriter fWriter(options.outputDir + "\\", false);
	std::vector<std::string> headline;
	uint cpuRowCounter = 1;
	uint gpuRowCounter = 1;
	uint fileRowCounter = 1;

	// initalize headline
	headline.push_back("Pixelcount");
	headline.push_back("Methode");
	headline.push_back("Sigma");
	headline.push_back("Time CPU in s");
	headline.push_back("Time GPU in s");
	headline.push_back("Picturename");

	// resizing FileWriter
	fWriter.resizeRows((options.inputFiles.size() + 1) * 2);
	fWriter.resizeColumns(headline.size());

	// writing headline to file
	fWriter.writeHeadline(headline);

	// initalize all images
	std::cout << "Loading Images:\n";
	for (int i = 0; i < options.inputFiles.size(); ++i)
	{
		originals.push_back(cv::imread(options.inputFiles[i]));
		//gaussian blur needs 4 channels
		if (originals[i].channels() == 3) cv::cvtColor(originals[i], originals[i], cv::COLOR_BGR2BGRA);
		if (options.gaussianBlur)
		{
			resultsGPUGuassianBlur.push_back(originals[i].clone());
			resultsCPUGaussianBlur.push_back(originals[i].clone());
		}

		std::cout << "   loaded file: " << options.fileNames[i] << '\n';
	}

	// writing resolution to file
	int i = 0;
	if (options.gaussianBlur) {
		for (cv::Mat image : originals) {
			fWriter.writeSingleValueToColumn(i + 1, 0, image.rows * image.cols);
			++i;
		}
	}
	if (options.conversionRGBToYCbCr) {
		for (cv::Mat image : originals) {
			fWriter.writeSingleValueToColumn(i + 1, 0, image.rows * image.cols);
			++i;
		}
	}

	// gaussian blur
	if (options.gaussianBlur)
	{
		std::cout << "Start Gaussian Blur Routine\n";
		int maskSize;
		float* mask = Mask::createBlurMask(options.sigma, &maskSize);
		t.start("GaussianBlur");
		for (int i = 0; i < originals.size(); ++i)
		{
			fWriter.writeTextToColumn(gpuRowCounter, 1, "Gaussian Blur");
			fWriter.writeSingleValueToColumn(gpuRowCounter, 2, options.sigma);
			OpenCLProgram prog("GaussianBlur.cl", "gaussianBlur");
			prog.setImage2D(0, (size_t)(originals[i].cols), (size_t)(originals[i].rows), (size_t)(originals[i].channels()), originals[i].data, true);
			prog.setImage2D(1, (size_t)(resultsGPUGuassianBlur[i].cols), (size_t)(resultsGPUGuassianBlur[i].rows), (size_t)(resultsGPUGuassianBlur[i].channels()), resultsGPUGuassianBlur[i].data, false);
			prog.setArg(2, mask, sizeof(float) * (maskSize * 2 + 1) * (maskSize * 2 + 1), true);
			prog.setArg(3, maskSize);
			prog.startProgram(cl::NDRange(originals[i].cols, originals[i].rows));
			prog.getResults(fWriter, gpuRowCounter, 4);
			++gpuRowCounter;
		}
		t.stop();

		t.start("GaussianBlur on CPU");
		for (int i = 0; i < originals.size(); ++i)
		{
			fWriter.writeTextToColumn(cpuRowCounter, 1, "Gaussian Blur");
			resultsCPUGaussianBlur[i] = process.blurWithGaus(originals[i], mask, maskSize, fWriter, cpuRowCounter, 3);
			++cpuRowCounter;
		}
		t.stop();

		for (int i = 0; i < originals.size(); ++i)
		{
			deltas.push_back(resultsGPUGuassianBlur[i] - resultsCPUGaussianBlur[i]);
		}
		delete[] mask;
	}

    // color conversion rgb to ycbcr

	// color conversion needs 3 channels
	if (options.conversionRGBToYCbCr)
	{
		for (int i = 0; i < originals.size(); ++i)
		{
			if (originals[i].channels() == 4)
				cv::cvtColor(originals[i], originals[i], cv::COLOR_BGRA2BGR);
			resultsGPUYCbCr.push_back(originals[i].clone());
			resultsCPUYCbCr.push_back(originals[i].clone());
		}
	}

	if (options.conversionRGBToYCbCr)
	{
		std::cout << "Start Color Conversion Routine\n";
		t.start("BGR zu YCrCb");
		for (int i = 0; i < originals.size(); ++i)
		{
			fWriter.writeTextToColumn(gpuRowCounter, 1, "BGR zu YCrCb");
			OpenCLProgram prog("CvtBGR2YCrCb_Kernel.cl", "convertBGR2YCrCb");
			prog.setArg(0, originals[i].data, (size_t)(originals[i].rows) * (size_t)(originals[i].cols) * (size_t)(originals[i].channels()), true);
			prog.setArg(1, resultsGPUYCbCr[i].data, (size_t)(resultsGPUYCbCr[i].rows) * (size_t)(resultsGPUYCbCr[i].cols) * (size_t)(resultsGPUYCbCr[i].channels()), false);
			prog.setArg(2, originals[i].cols * originals[i].channels());
			prog.startProgram(cl::NDRange(originals[i].cols, originals[i].rows));
			prog.getResults(fWriter, gpuRowCounter, 4);
			++gpuRowCounter;
		}
		t.stop();

		t.start("BGR zu YCrCb on CPU");
		for (int i = 0; i < originals.size(); ++i)
		{
			fWriter.writeTextToColumn(cpuRowCounter, 1, "BGR zu YCrCb");
			resultsCPUYCbCr[i] = process.convertBGR2YCrCb(originals[i], fWriter, cpuRowCounter, 3);
			++cpuRowCounter;
		}
		t.stop();

		for (int i = 0; i < originals.size(); ++i)
		{
			deltas.push_back(resultsGPUYCbCr[i] - resultsCPUYCbCr[i]);
		}
	}

	//output all
	std::cout << "Saving results...\n";
	for (int i = 0; i < resultsGPUGuassianBlur.size(); ++i)
	{
		cv::imwrite(options.outputDir + "\\GaussianBlur_GPU_" + options.fileNames[i], resultsGPUGuassianBlur[i]);
		std::cout << "   " << options.outputDir + "\\GaussianBlur_GPU_" + options.fileNames[i] << '\n';

		cv::imwrite(options.outputDir + "\\GaussianBlur_CPU_" + options.fileNames[i], resultsCPUGaussianBlur[i]);
		std::cout << "   " << options.outputDir + "\\GaussianBlur_CPU_" + options.fileNames[i] << '\n';

		cv::imwrite(options.outputDir + "\\Deltas_GaussianBlur_" + options.fileNames[i], deltas[i]);
		std::cout << "   " << options.outputDir + "\\Deltas_GaussianBlur_" + options.fileNames[i] << '\n';

		fWriter.writeTextToColumn(fileRowCounter, 5, options.fileNames[i]);
		++fileRowCounter;
	}
	for (int i = 0; i < resultsGPUYCbCr.size(); ++i)
	{
		cv::imwrite(options.outputDir + "\\YCbCr_GPU_" + options.fileNames[i], resultsGPUYCbCr[i]);
		std::cout << "   " << options.outputDir + "\\YCbCr_GPU_" + options.fileNames[i] << '\n';

		cv::imwrite(options.outputDir + "\\YCbCr_CPU_" + options.fileNames[i], resultsCPUYCbCr[i]);
		std::cout << "   " << options.outputDir + "\\YCbCr_CPU_" + options.fileNames[i] << '\n';

		if (options.gaussianBlur) 
			cv::imwrite(options.outputDir + "\\Deltas_YCbCr_" + options.fileNames[i], deltas[i + resultsGPUYCbCr.size() - 1]);
		else
			cv::imwrite(options.outputDir + "\\Deltas_YCbCr_" + options.fileNames[i], deltas[i]);
		std::cout << "   " << options.outputDir + "\\Deltas_YCbCr_" + options.fileNames[i] << '\n';

		fWriter.writeTextToColumn(fileRowCounter, 5, options.fileNames[i]);
		++fileRowCounter;
	}
	std::cout << "finished\n";
	fWriter.closeFile();
	return 0;
}