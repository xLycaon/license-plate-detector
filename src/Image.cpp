#include "Image.hpp"
#include <stdexcept>
#include <opencv2/imgcodecs.hpp>

Image::Image(const std::string& path) : imagePath(path) {
    // Load the original image from the given path
    original = cv::imread(path);
    if (original.empty()) {
        throw std::runtime_error("Failed to load image: " + path);
    }
    loadCascades();  // Load the Haar cascades for license plate detection
}

Image::Image(const std::vector<cv::Mat>& frames) : frames(frames) {
	loadCascades();  // Load Haar cascades when initialized with a vector of frames
}

Image::Image(const cv::Mat& img) : original(img) {
    loadCascades();  // Load Haar cascades when initialized with a cv::Mat image
}

Image::Image() {
	loadCascades();  // Load Haar cascades when initialized with no arguments
}

void Image::loadCascades() {
    cascadeVector.resize(3);  // Resize the vector to hold 6 cascades

    // Paths to the Haar cascade files
    std::string cascadePath1 = HAAARCASCADES_PATH "\\haarcascade_russian_plate_number.xml";
    std::string cascadePath2 = HAAARCASCADES_PATH "\\eu.xml";
    std::string cascadePath3 = HAAARCASCADES_PATH "\\haarcascade_license_plate_rus_16stages.xml";

    // Load each cascade and handle errors
    if (!cascadeVector[0].load(cascadePath1)) {
        std::cerr << "Error loading cascade 1!" << std::endl;
    }
 
    if (!cascadeVector[1].load(cascadePath2)) {
        std::cerr << "Error loading cascade 2!" << std::endl;
    }

    if (!cascadeVector[2].load(cascadePath3)) {
        std::cerr << "Error loading cascade 3!" << std::endl;
    }
  
}

void Image::addFrame(const cv::Mat& frame) {
    frames.push_back(frame);  // Add a new frame to the frames vector
}

bool Image::checkImage(const cv::Mat& frame) {
    return !frame.empty() && frame.channels() == 1;  // Check if the frame is non-empty and grayscale
}

void Image::resizeFrames(int width, int height) {
    for (auto& frame : frames) {
        cv::resize(frame, frame, cv::Size(width, height));  // Resize all frames to the specified width and height
    }
}

cv::Mat Image::getFrame(size_t index) {
    if (index >= frames.size()) {
        throw std::out_of_range("Frame index out of range.");  // Ensure the index is within range
    }
    return frames[index];  // Return the frame at the specified index
}

void Image::saveFrames(const std::string& folderPath) {
    for (size_t i = 0; i < frames.size(); ++i) {
        std::string filename = folderPath + "/frame_" + std::to_string(i) + ".jpg";
        cv::imwrite(filename, frames[i]);  // Save each frame as a .jpg file
    }
}


bool Image::detectLicensePlate(const cv::Mat& frame, std::vector<cv::Rect>& detectedPlates) {
    if (frame.empty()) {
        std::cerr << "Input frame is empty!" << std::endl;
        return false;
    }

    cv::Mat gray;
    cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);  // Convert to grayscale

    for (auto& cascade : cascadeVector) {
        if (cascade.empty()) {
            std::cerr << "Cascade classifier not loaded!" << std::endl;
            continue;
        }

        std::vector<cv::Rect> plates;
        cascade.detectMultiScale(gray, plates, 1.1, 5, cv::CASCADE_SCALE_IMAGE);

        detectedPlates.insert(detectedPlates.end(), plates.begin(), plates.end());
    }

    return !detectedPlates.empty();
}

cv::Mat Image::rotateImage(const cv::Mat& image, float angle) {
    cv::Point2f center(image.cols / 2.0f, image.rows / 2.0f);
    cv::Mat rotMat = cv::getRotationMatrix2D(center, angle, 1.0);  // Create rotation matrix
    cv::Mat rotated;
    cv::warpAffine(image, rotated, rotMat, image.size(), cv::INTER_LINEAR);  // Apply rotation to the image
    return rotated;  // Return the rotated image
}