#ifndef IMAGEPROCESSOR_HPP
#define IMAGEPROCESSOR_HPP

#pragma once

#include "Image.hpp"
#include <opencv2/opencv.hpp>
#include <vector>
#include <unordered_set>
#include <map>
#include <thread>
#include <mutex>
#include <atomic>
#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>


class ImageProcessor {
public:
    ImageProcessor();
    ~ImageProcessor();

    static double averageConfidence;
    static int numberOfFiles;

    std::string process_Extract_Adaptive(const cv::Mat& frame, std::vector<cv::Rect>& plateRegions);
    std::string ImageProcessor::findMostSimilarLicensePlate(const std::string& detectedText, const std::string& detectedText1);
    std::string test_OCR_grayScale(const cv::Mat& frame, std::vector<cv::Rect>& plateRegions);
    void ImageProcessor::showAverageConfidence(double confidence, int numberOfPictures);
    
private:
    std::unordered_set<std::string> licensePlates;
    
    std::map<char, std::vector<cv::Mat>> templates;
    cv::dnn::Net net;
    std::mutex ocrMutex; 
    std::atomic<int> successfulThreads{ 0 };
    
};

#endif