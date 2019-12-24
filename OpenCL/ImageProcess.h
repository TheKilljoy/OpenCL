#pragma once

class ImageProcess {
public:
	ImageProcess();
	~ImageProcess();

	cv::Mat blurWithGaus(const cv::Mat& originalImage, const float* mask, const int maskSize) const;
};