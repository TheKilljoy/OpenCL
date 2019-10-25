#include "pch.h"
#include "OpenCLProgram.h"
#include "CL/cl.hpp"
#include "Timer.h"
#include "opencv2/opencv.hpp"

int main(int argc, char** argv)
{
	std::string currentFolder = argv[0];
	currentFolder = currentFolder.substr(0, currentFolder.find_last_of("\\"));
	std::string imgFile = currentFolder + "\\pngs\\dice.png";
	// std::string imgFile = currentFolder + "\\pngs\\test.png";
	cv::Mat	originalImg = cv::imread(imgFile);
	if (originalImg.data == nullptr)
	{
		std::cout << "Image file not found!\n";
		std::exit(1);
	}

	if (originalImg.channels() != 4)
	{
		cv::cvtColor(originalImg, originalImg, cv::COLOR_BGR2BGRA);
	}

	cv::Mat outImg = originalImg.clone();

	// OpenCLProgram prog("Grayscale_Kernel.cl", "grayScale");
	OpenCLProgram prog("YCbCr_Kernel.cl", "convertRgbToYcbcr");
	prog.setImage2D(0, originalImg.cols, originalImg.rows, (size_t) (originalImg.cols) * (size_t)(originalImg.channels()), originalImg.data, true);
	prog.setImage2D(1, originalImg.cols, originalImg.rows, (size_t) (originalImg.cols) * (size_t)(originalImg.channels()), outImg.data, false);

	prog.startProgram(cl::NDRange(originalImg.cols, originalImg.rows));
	std::vector<void*> result = prog.getResults();
	
	cv::imwrite(currentFolder + "\\Original.png", originalImg);
	cv::imwrite(currentFolder + "\\Result.png", outImg);
	return 0;
}