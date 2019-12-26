#include "pch.h"
#include "opencv2/opencv.hpp"
#include "ImageProcess.h"

ImageProcess::ImageProcess(){}
ImageProcess::~ImageProcess(){}

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
						sum_b += mask[a + maskSize + (b + maskSize) * (maskSize * 2 + 1)] * originalImage.data[y * originalImage.cols * channels + x * channels + 0];
						sum_g += mask[a + maskSize + (b + maskSize) * (maskSize * 2 + 1)] * originalImage.data[y * originalImage.cols * channels + x * channels + 1];
						sum_r += mask[a + maskSize + (b + maskSize) * (maskSize * 2 + 1)] * originalImage.data[y * originalImage.cols * channels + x * channels + 2];
					}
					else if (x + b < 0 || x + b >= originalImage.cols) {
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

cv::Mat ImageProcess::convertBGR2YCrCb(const cv::Mat& originalImage) const
{
	cv::Mat outImage = originalImage.clone();
	int channels = originalImage.channels();
	uint b, g, r, y;

	for (int iy = 0; iy < originalImage.rows; iy++) {
		for (int ix = 0; ix < originalImage.cols; ix++) {
			b = originalImage.data[iy * originalImage.cols * channels + ix * channels + 0];
			g = originalImage.data[iy * originalImage.cols * channels + ix * channels + 1];
			r = originalImage.data[iy * originalImage.cols * channels + ix * channels + 2];
			y = 0.299 * r + 0.587 * g + 0.114 * b;
			outImage.data[iy * originalImage.cols * channels + ix * channels + 0] = y;
			outImage.data[iy * originalImage.cols * channels + ix * channels + 1] = (r - y) * 0.713 + 128;
			outImage.data[iy * originalImage.cols * channels + ix * channels + 2] = (b - y) * 0.564 + 128;
		}
	}
	
	return outImage;
}