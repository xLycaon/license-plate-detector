#ifndef IMAGEPROCESSOR_HPP
#define IMAGEPROCESSOR_HPP

#pragma once

#include "Image.hpp"
#include <opencv2/opencv.hpp>
#include <vector>
#include <unordered_set>

class ImageProcessor {
public:
	ImageProcessor();

	~ImageProcessor();

	void processFrames(const std::vector<cv::Mat>& frames);

	cv::Mat preprocessImage(const cv::Mat& frame);

	bool detectLicensePlate(const cv::Mat& frame, cv::Rect& plateRegion);

	std::string analyzeMultipleFramesForPlate(const std::vector<cv::Mat>& frames);

private:
	std::unordered_set<std::string> licensePlates;
	cv::CascadeClassifier plateCascade1;
	cv::CascadeClassifier plateCascade2;
	bool isDuplicate(const std::string& plateNumber);
	std::string processLicensePlate(const cv::Mat& plateRegion);
};

#endif