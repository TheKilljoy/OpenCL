#pragma once

namespace cv 
{
	class Mat;
}

class FileWriter;

class ImageProcess
{
public:
	ImageProcess();
	~ImageProcess();

	cv::Mat blurWithGaus(const cv::Mat& originalImage, const float* mask, const int maskSize, FileWriter& fWriter, const int row, const int col) const;
	cv::Mat convertBGR2YCrCb(const cv::Mat& originalImage, FileWriter& fWriter, const int row, const int col) const;
private:
	std::unique_ptr<Timer> timer;
};