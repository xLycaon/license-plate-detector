#ifndef IMAGE_HPP
#define IMAGE_HPP

#pragma once

#include <string>
#include <vector>
#include <opencv2/opencv.hpp>


struct Image {


    std::string imagePath;
    cv::Mat original;
    std::vector<cv::Mat> frames;

    Image(const std::string& path);
    Image(const cv::Mat& img);

    void addFrame(const cv::Mat& frame);
    bool checkImage(const cv::Mat& frame);
    void resizeFrames(int width, int height);
    cv::Mat getFrame(size_t index);
    void saveFrames(const std::string& folderPath);

};

#endif
