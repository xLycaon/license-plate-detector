#ifndef IMAGE_HPP
#define IMAGE_HPP

#pragma once

#include <string>
#include <vector>
#include <opencv2/opencv.hpp>


class Image {

public:

    std::string imagePath;
    cv::Mat original;
    std::vector<cv::Mat> frames;
    cv::Mat getFrame(size_t index);
    std::vector<cv::CascadeClassifier> cascadeVector;

    Image(const std::string& path);
    Image(const cv::Mat& img);
    Image(const std::vector<cv::Mat>& frames);
    Image();

    void addFrame(const cv::Mat& frame);
    bool checkImage(const cv::Mat& frame);
    void resizeFrames(int width, int height);
    cv::Mat rotateImage(const cv::Mat& image, float angle);
    void saveFrames(const std::string& folderPath);
    bool detectLicensePlate(const cv::Mat& frame, std::vector<cv::Rect>& detectedPlates);
    void loadCascades();


private:
    const std::string haarcascade_PATH = HAAARCASCADES_PATH;
};

#endif
