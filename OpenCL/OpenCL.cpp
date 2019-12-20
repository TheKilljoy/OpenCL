#include "pch.h"
#include "OpenCLProgram.h"
#include "CL/cl.hpp"
#include "Timer.h"
#include "Mask.h"
#include "Options.h"
#include "ArgumentParser.h"

#include "opencv2/opencv.hpp"



int main(int argc, char** argv)
{
	//ArgumentParser parser;
	//Options options;
	//// check arguments
	//if (parser.parseArguments(argc, argv, options))
	//{
	//	std::cout << "Success\n";
	//}

	std::string currentFolder = argv[0];
	currentFolder = currentFolder.substr(0, currentFolder.find_last_of("\\"));
	std::string imgFile = currentFolder + "\\jpgs\\Barns_grand_tetons.jpg";
	cv::Mat	originalImg = cv::imread(imgFile);
	if (originalImg.data == nullptr)
	{
		std::cout << "Image file not found!\n";
		std::exit(1);
	}

	if (originalImg.channels() == 3)
	{
		cv::cvtColor(originalImg, originalImg, cv::COLOR_BGR2BGRA);
	}

	cv::Mat outImg = originalImg.clone();
	// cv::Mat outImg(originalImg.rows, originalImg.cols, CV_8UC3);
	cv::Mat openCV2YCrCb = originalImg.clone();
	cv::Mat openCVGaus;

	Timer t;

	int maskSize;
	float* mask = Mask::createBlurMask(2.0f, &maskSize);

	/*
	printf("Maske\n");
	for (int a = -maskSize; a < maskSize + 1; a++) {
		for (int b = -maskSize; b < maskSize + 1; b++) {
			printf("%f, ",mask[a + maskSize + (b + maskSize) * (maskSize * 2 + 1)]);
		}
	}
	printf("\n"); 
	*/
	/*
	t.start("GaussianBlur");
	OpenCLProgram prog("GaussianBlur.cl", "gaussianBlur");
	prog.setImage2D(0, (size_t)(originalImg.cols), (size_t)(originalImg.rows), (size_t)(originalImg.channels()), originalImg.data, true);
	prog.setImage2D(1, (size_t)(outImg.cols), (size_t)(outImg.rows), (size_t)(outImg.channels()), outImg.data, false);
	prog.setArg(2, mask, sizeof(float) * (maskSize * 2 + 1) * (maskSize * 2 + 1), true);
	prog.setArg(3, maskSize);
	prog.startProgram(cl::NDRange(originalImg.cols, originalImg.rows));
	prog.getResults();
	t.stop();
	*/

	printf("Bild, blue channel\n");
	int channels = originalImg.channels();
	float sum_b, sum_g, sum_r;
	for (int y = 0; y < originalImg.rows; y++) {
		for (int x = 0; x < originalImg.cols; x++) {
			sum_b = 0.0f;
			sum_g = 0.0f;
			sum_r = 0.0f;
			for (int a = -maskSize; a < maskSize + 1; a++) {
				for (int b = -maskSize; b < maskSize + 1; b++) {
					if (y + a < 0 || y + a >= originalImg.rows) {
						continue;
					}
					else if (x + b < 0 || x + b >= originalImg.cols) {
						continue;
					}
					else {
						sum_b += mask[a + maskSize + (b + maskSize) * (maskSize * 2 + 1)] * originalImg.data[(y + a) * originalImg.cols * channels + (x + b) * channels + 0];
						sum_g += mask[a + maskSize + (b + maskSize) * (maskSize * 2 + 1)] * originalImg.data[(y + a) * originalImg.cols * channels + (x + b) * channels + 1];
						sum_r += mask[a + maskSize + (b + maskSize) * (maskSize * 2 + 1)] * originalImg.data[(y + a) * originalImg.cols * channels + (x + b) * channels + 2];
					}
				}
			}
			outImg.data[y * originalImg.cols * channels + x * channels + 0] = (uint8_t)(sum_b);
			outImg.data[y * originalImg.cols * channels + x * channels + 1] = (uint8_t)(sum_g);
			outImg.data[y * originalImg.cols * channels + x * channels + 2] = (uint8_t)(sum_r);
		}
	}
	

	cv::GaussianBlur(originalImg, openCVGaus, cv::Size(5, 5), 2, 2);

	cv::Mat delta(originalImg.rows, originalImg.cols, CV_8UC3);
	delta = originalImg - openCVGaus;

	cv::imwrite(currentFolder + "\\original.png", originalImg);
	cv::imwrite(currentFolder + "\\Out_Blur.png", outImg);
	cv::imwrite(currentFolder + "\\delta.png", delta);
	cv::imwrite(currentFolder + "\\Gaus.png", openCVGaus);
	delete mask;

	/*
	t.start("BGR zu YCrCb");
	OpenCLProgram prog("CvtBGR2YCrCb_Kernel.cl", "convertBGR2YCrCb");
	prog.setArg(0, originalImg.data, (size_t)(originalImg.rows) * (size_t)(originalImg.cols) * (size_t)(originalImg.channels()), true);
	prog.setArg(1, outImg.data, (size_t)(outImg.rows) * (size_t)(outImg.cols) * (size_t)(outImg.channels()), false);
	prog.setArg(2, originalImg.cols * originalImg.channels());
	prog.startProgram(cl::NDRange(originalImg.cols, originalImg.rows));
	prog.getResults();
	t.stop();


	t.start("OpenCV BGR zu YCrCb");
	cv::cvtColor(originalImg, openCV2YCrCb, cv::COLOR_BGR2YCrCb);
	t.stop();

	cv::Mat delta(originalImg.rows, originalImg.cols, CV_8UC3);
	cv::Mat backToBGR(originalImg.rows, originalImg.cols, CV_8UC3);
	delta = originalImg - outImg;

	t.start("YCrCb zu BGR");
	OpenCLProgram prog2("CvtYCrCb2BGR_Kernel.cl", "convertYCrCb2BGR");
	prog2.setArg(0, outImg.data, (size_t)(outImg.rows) * (size_t)(outImg.cols) * (size_t)(outImg.channels()), true);
	prog2.setArg(1, backToBGR.data, (size_t)(backToBGR.rows) * (size_t)(backToBGR.cols) * (size_t)(backToBGR.channels()), false);
	prog2.setArg(2, outImg.cols * outImg.channels());
	prog2.startProgram(cl::NDRange(outImg.cols, outImg.rows));
	prog2.getResults();
	t.stop();

	cv::imwrite(currentFolder + "\\Out_Delta.png", delta);
	cv::imwrite(currentFolder + "\\Out_YCrCb.png", outImg);
	cv::imwrite(currentFolder + "\\Out_CV2_YCrCb.png", originalImg);
	cv::imwrite(currentFolder + "\\Out_BGR_From_YCrCb.png", backToBGR);
	*/
	return 0;
}