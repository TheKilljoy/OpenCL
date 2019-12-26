#include "pch.h"
#include "opencv2/opencv.hpp"
#include "Timer.h"
#include "FileWriter.h"
#include "ImageProcess.h"

#define BLUE 0
#define GREEN 1
#define RED 2

ImageProcess::ImageProcess()
{
	timer = std::make_unique<Timer>();
}
ImageProcess::~ImageProcess(){}

cv::Mat ImageProcess::blurWithGaus(const cv::Mat& originalImage, const float* mask, const int maskSize, FileWriter& fWriter, const int row, const int col) const
{
	timer->start("CPU calculations");
	cv::Mat outImage = originalImage.clone();
	int channels = originalImage.channels();
	float sum_b, sum_g, sum_r;
	
	for (int y = 0; y < originalImage.rows; y++) {
		for (int x = 0; x < originalImage.cols; x++) {
			sum_b = 0.0f;
			sum_g = 0.0f;
			sum_r = 0.0f;
			for (int a = -maskSize; a < maskSize + 1; a++) {
				for (int b = -maskSize; b < maskSize + 1; b++) {
					if (y + a < 0 || y + a >= originalImage.rows) {
						sum_b += mask[a + maskSize + (b + maskSize) * (maskSize * 2 + 1)] * originalImage.data[y * originalImage.cols * channels + x * channels + BLUE];
						sum_g += mask[a + maskSize + (b + maskSize) * (maskSize * 2 + 1)] * originalImage.data[y * originalImage.cols * channels + x * channels + GREEN];
						sum_r += mask[a + maskSize + (b + maskSize) * (maskSize * 2 + 1)] * originalImage.data[y * originalImage.cols * channels + x * channels + RED];
					}
					else if (x + b < 0 || x + b >= originalImage.cols) {
						sum_b += mask[a + maskSize + (b + maskSize) * (maskSize * 2 + 1)] * originalImage.data[y * originalImage.cols * channels + x * channels + BLUE];
						sum_g += mask[a + maskSize + (b + maskSize) * (maskSize * 2 + 1)] * originalImage.data[y * originalImage.cols * channels + x * channels + GREEN];
						sum_r += mask[a + maskSize + (b + maskSize) * (maskSize * 2 + 1)] * originalImage.data[y * originalImage.cols * channels + x * channels + RED];
					}
					else {
						sum_b += mask[a + maskSize + (b + maskSize) * (maskSize * 2 + 1)] * originalImage.data[(y + a) * originalImage.cols * channels + (x + b) * channels + BLUE];
						sum_g += mask[a + maskSize + (b + maskSize) * (maskSize * 2 + 1)] * originalImage.data[(y + a) * originalImage.cols * channels + (x + b) * channels + GREEN];
						sum_r += mask[a + maskSize + (b + maskSize) * (maskSize * 2 + 1)] * originalImage.data[(y + a) * originalImage.cols * channels + (x + b) * channels + RED];
					}
				}
			}
			outImage.data[y * originalImage.cols * channels + x * channels + BLUE] = (uint8_t)(sum_b);
			outImage.data[y * originalImage.cols * channels + x * channels + GREEN] = (uint8_t)(sum_g);
			outImage.data[y * originalImage.cols * channels + x * channels + RED] = (uint8_t)(sum_r);
		}
	}

	fWriter.writeSingleValueToColumn(row, col, timer->stop());
	return outImage;
}

cv::Mat ImageProcess::convertBGR2YCrCb(const cv::Mat& originalImage, FileWriter& fWriter, const int row, const int col) const
{
	timer->start("CPU calculations");
	cv::Mat outImage = originalImage.clone();
	int channels = originalImage.channels();
	uint8_t b, g, r, y;

	for (int iy = 0; iy < originalImage.rows; iy++) {
		for (int ix = 0; ix < originalImage.cols; ix++) {
			b = originalImage.data[iy * originalImage.cols * channels + ix * channels + BLUE];
			g = originalImage.data[iy * originalImage.cols * channels + ix * channels + GREEN];
			r = originalImage.data[iy * originalImage.cols * channels + ix * channels + RED];
			y = 0.299 * r + 0.587 * g + 0.114 * b;
			outImage.data[iy * originalImage.cols * channels + ix * channels + BLUE] = y;
			outImage.data[iy * originalImage.cols * channels + ix * channels + GREEN] = (r - y) * 0.713 + 128;
			outImage.data[iy * originalImage.cols * channels + ix * channels + RED] = (b - y) * 0.564 + 128;
		}
	}
	
	fWriter.writeSingleValueToColumn(row, col, timer->stop());
	return outImage;
}