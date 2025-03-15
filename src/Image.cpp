#include "Image.hpp"
#include <stdexcept>
#include <opencv2/imgcodecs.hpp>

Image::Image(const std::string& path) : imagePath(path) {
    original = cv::imread(path);
    if (original.empty()) {
        throw std::runtime_error("Failed to load image: " + path);
    }
}

Image::Image(const cv::Mat& img) : original(img) {}

void Image::addFrame(const cv::Mat& frame) {
    frames.push_back(frame);
}

bool Image::checkImage(const cv::Mat& frame) {
    return !frame.empty() && frame.channels() == 1;
}

void Image::resizeFrames(int width, int height) {
    for (auto& frame : frames) {
        cv::resize(frame, frame, cv::Size(width, height));
    }
}

cv::Mat Image::getFrame(size_t index) {
    if (index >= frames.size()) {
        throw std::out_of_range("Frame index out of range.");
    }
    return frames[index];
}

void Image::saveFrames(const std::string& folderPath) {
    for (size_t i = 0; i < frames.size(); ++i) {
        std::string filename = folderPath + "/frame_" + std::to_string(i) + ".jpg";
        cv::imwrite(filename, frames[i]);
    }
}