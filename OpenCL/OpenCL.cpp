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
  
	std::vector<cv::Mat> originals;
	std::vector<cv::Mat> resultsGuassianBlur;
	std::vector<cv::Mat> resultsYCbCr;
	std::vector<cv::Mat> resultsOpenCVGaussianBlur;
	std::vector<cv::Mat> resultsOpenCVYCbCr;
	std::vector<cv::Mat> deltas;
	Timer t;

	// initalize all images
	std::cout << "Loading Images:\n";
	for (int i = 0; i < options.inputFiles.size(); ++i)
	{
		originals.push_back(cv::imread(options.inputFiles[i]));
		if (originals[i].channels() == 3) cv::cvtColor(originals[i], originals[i], cv::COLOR_BGR2BGRA);
		if (options.gaussianBlur)
		{
			resultsGuassianBlur.push_back(originals[i].clone());
			resultsOpenCVGaussianBlur.push_back(originals[i].clone());
		}
		if (options.conversionRGBToYCbCr)
		{
			resultsYCbCr.push_back(originals[i].clone());
			resultsOpenCVYCbCr.push_back(originals[i].clone());
		}
		std::cout << "   loaded file: " << options.fileNames[i] << '\n';
	}

	// gaussian blur
	if (options.gaussianBlur)
	{
		std::cout << "Start Gaussian Blur Routine\n";
		int maskSize;
		float* mask = Mask::createBlurMask(2.0f, &maskSize);
		t.start("GaussianBlur");
		for (int i = 0; i < originals.size(); ++i)
		{
			OpenCLProgram prog("GaussianBlur.cl", "gaussianBlur");
			prog.setImage2D(0, (size_t)(originals[i].cols), (size_t)(originals[i].rows), (size_t)(originals[i].channels()), originals[i].data, true);
			prog.setImage2D(1, (size_t)(resultsGuassianBlur[i].cols), (size_t)(resultsGuassianBlur[i].rows), (size_t)(resultsGuassianBlur[i].channels()), resultsGuassianBlur[i].data, false);
			prog.setArg(2, mask, sizeof(float) * (maskSize * 2 + 1) * (maskSize * 2 + 1), true);
			prog.setArg(3, maskSize);
			prog.startProgram(cl::NDRange(originals[i].cols, originals[i].rows));
			prog.getResults();
		}
		t.stop();

		t.start("GaussianBlur OpenCV");
		for (int i = 0; i < originals.size(); ++i)
		{
			cv::GaussianBlur(originals[i], resultsOpenCVGaussianBlur[i], cv::Size(5, 5), 2, 2);
		}
		t.stop();

		for (int i = 0; i < originals.size(); ++i)
		{
			deltas.push_back(resultsGuassianBlur[i] - resultsOpenCVGaussianBlur[i]);
		}
		delete[] mask;
	}

  // color conversion rgb to ycbcr
	if (options.conversionRGBToYCbCr)
	{
		std::cout << "Start Color Conversion Routine\n";
		t.start("BGR zu YCrCb");
		for (int i = 0; i < originals.size(); ++i)
		{
			OpenCLProgram prog("CvtBGR2YCrCb_Kernel.cl", "convertBGR2YCrCb");
			prog.setArg(0, originals[i].data, (size_t)(originals[i].rows) * (size_t)(originals[i].cols) * (size_t)(originals[i].channels()), true);
			prog.setArg(1, resultsYCbCr[i].data, (size_t)(resultsYCbCr[i].rows) * (size_t)(resultsYCbCr[i].cols) * (size_t)(resultsYCbCr[i].channels()), false);
			prog.setArg(2, originals[i].cols * originals[i].channels());
			prog.startProgram(cl::NDRange(originals[i].cols, originals[i].rows));
			prog.getResults();
		}
		t.stop();

		t.start("BGR zu YCrCb OpenCV");
		for (int i = 0; i < originals.size(); ++i)
		{
			cv::cvtColor(originals[i], resultsOpenCVYCbCr[i], cv::COLOR_BGR2YCrCb);
		}
		t.stop();

		for (int i = 0; i < originals.size(); ++i)
		{
			//openCV ycbcr conversion always removes the alpha channel, so it has to be restored
			cv::cvtColor(resultsOpenCVYCbCr[i], resultsOpenCVYCbCr[i], cv::COLOR_BGR2BGRA);
			deltas.push_back(resultsYCbCr[i] - resultsOpenCVYCbCr[i]);
		}
	}

	//output all
	std::cout << "Saving results...\n";
	for (int i = 0; i < resultsGuassianBlur.size(); ++i)
	{
		cv::imwrite(options.outputDir + "\\GaussianBlur_" + options.fileNames[i], resultsGuassianBlur[i]);
		std::cout << "   " << options.outputDir + "\\GaussianBlur_" + options.fileNames[i] << '\n';
		cv::imwrite(options.outputDir + "\\Deltas_GaussianBlur_" + options.fileNames[i], deltas[i]);
		std::cout << "   " << options.outputDir + "\\Deltas_GaussianBlur_" + options.fileNames[i] << '\n';
	}
	for (int i = 0; i < resultsOpenCVYCbCr.size(); ++i)
	{
		cv::imwrite(options.outputDir + "\\YCbCr_" + options.fileNames[i], resultsOpenCVYCbCr[i]);
		std::cout << "   " << options.outputDir + "\\YCbCr_" + options.fileNames[i] << '\n';
		if (options.gaussianBlur) 
			cv::imwrite(options.outputDir + "\\Deltas_YCbCr_" + options.fileNames[i], deltas[i + resultsOpenCVYCbCr.size() - 1]);
		else
			cv::imwrite(options.outputDir + "\\Deltas_YCbCr_" + options.fileNames[i], deltas[i]);
		std::cout << "   " << options.outputDir + "\\Deltas_YCbCr_" + options.fileNames[i] << '\n';
	}
	std::cout << "finished\n";
	return 0;
}