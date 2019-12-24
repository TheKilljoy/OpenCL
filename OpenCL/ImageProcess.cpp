#include "ImageProcess.h"
#include "opencv2/opencv.hpp"
#include "pch.h"

ImageProcess::ImageProcess() { }
ImageProcess::~ImageProcess() { }

cv::Mat ImageProcess::blurWithGaus(const cv::Mat& originalImage, const float* mask, const int maskSize) const
{
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
						// continue;
						sum_b += mask[a + maskSize + (b + maskSize) * (maskSize * 2 + 1)] * originalImage.data[y * originalImage.cols * channels + x * channels + 0];
						sum_g += mask[a + maskSize + (b + maskSize) * (maskSize * 2 + 1)] * originalImage.data[y * originalImage.cols * channels + x * channels + 1];
						sum_r += mask[a + maskSize + (b + maskSize) * (maskSize * 2 + 1)] * originalImage.data[y * originalImage.cols * channels + x * channels + 2];
					}
					else if (x + b < 0 || x + b >= originalImage.cols) {
						// continue;
						sum_b += mask[a + maskSize + (b + maskSize) * (maskSize * 2 + 1)] * originalImage.data[y * originalImage.cols * channels + x * channels + 0];
						sum_g += mask[a + maskSize + (b + maskSize) * (maskSize * 2 + 1)] * originalImage.data[y * originalImage.cols * channels + x * channels + 1];
						sum_r += mask[a + maskSize + (b + maskSize) * (maskSize * 2 + 1)] * originalImage.data[y * originalImage.cols * channels + x * channels + 2];
					}
					else {
						sum_b += mask[a + maskSize + (b + maskSize) * (maskSize * 2 + 1)] * originalImage.data[(y + a) * originalImage.cols * channels + (x + b) * channels + 0];
						sum_g += mask[a + maskSize + (b + maskSize) * (maskSize * 2 + 1)] * originalImage.data[(y + a) * originalImage.cols * channels + (x + b) * channels + 1];
						sum_r += mask[a + maskSize + (b + maskSize) * (maskSize * 2 + 1)] * originalImage.data[(y + a) * originalImage.cols * channels + (x + b) * channels + 2];
					}
				}
			}
			outImage.data[y * originalImage.cols * channels + x * channels + 0] = (uint8_t)(sum_b);
			outImage.data[y * originalImage.cols * channels + x * channels + 1] = (uint8_t)(sum_g);
			outImage.data[y * originalImage.cols * channels + x * channels + 2] = (uint8_t)(sum_r);
		}
	}

	return outImage;
}