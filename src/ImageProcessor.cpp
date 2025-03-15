#include "ImageProcessor.hpp"


#include <opencv2/opencv.hpp>
#include <vector>
#include <string>


ImageProcessor::ImageProcessor() {
    
    //classic haarcascade
    std::string cascadePath1 = HAAARCASCADES_PATH "/haarcascade_russian_plate_number.xml";
    if (!plateCascade1.load(cascadePath1)) {
        std::cerr << "Error loading first cascade classifier!" << std::endl;
    }

    //LBP haarcascade
    std::string cascadePath2 = HAAARCASCADES_PATH "/license_plate_rus_16stages.xml";
    if (!plateCascade2.load(cascadePath2)) {
        std::cerr << "Error loading second cascade classifier!" << std::endl;
    }
}
ImageProcessor::~ImageProcessor() {}

void ImageProcessor::processFrames(const std::vector<cv::Mat>& frames) {
    for (const auto& frame : frames) {

        cv::Rect plateRegion;
        if (detectLicensePlate(frame, plateRegion)) {
            std::string plateNumber = processLicensePlate(frame(plateRegion));

            if (!isDuplicate(plateNumber)) {
                licensePlates.insert(plateNumber);
                std::cout << "Detected license plate: " << plateNumber << std::endl;
            }
            else {
                std::cout << "Duplicate license plate detected: " << plateNumber << std::endl;
            }
        }
    }
}

std::string ImageProcessor::analyzeMultipleFramesForPlate(const std::vector<cv::Mat>& frames) {
    std::vector<std::string> detectedPlates;
    
    for (const auto& frame : frames) {
        cv::Rect plateRegion;  

        if (detectLicensePlate(frame, plateRegion)) {
            std::string plateNumber = processLicensePlate(frame(plateRegion));
            detectedPlates.push_back(plateNumber);
        }
    }

    if (detectedPlates.empty()) {
        return "";
    }

    std::unordered_map<std::string, int> plateVotes;
    for (const auto& plate : detectedPlates) {
        plateVotes[plate]++;
    }

    std::string bestPlate;
    int maxVotes = 0;
    for (const auto& vote : plateVotes) {
        if (vote.second > maxVotes) {
            maxVotes = vote.second;
            bestPlate = vote.first;
        }
    }

    return bestPlate;
}

cv::Mat ImageProcessor::preprocessImage(const cv::Mat& frame) {
    cv::Mat gray;
    cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);

    cv::Mat blurred;
    cv::GaussianBlur(gray, blurred, cv::Size(5, 5), 0);

    cv::Mat thresh;
    cv::threshold(blurred, thresh, 0, 255, cv::THRESH_BINARY + cv::THRESH_OTSU);

    cv::Mat edges;
    cv::Canny(thresh, edges, 100, 200);

    cv::Mat dilated;
    cv::dilate(edges, dilated, cv::Mat(), cv::Point(-1, -1), 2);

    return dilated;
}

bool ImageProcessor::detectLicensePlate(const cv::Mat& frame, cv::Rect& plateRegion) {
    cv::Mat gray;
    cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);

    std::vector<cv::Rect> plates1, plates2;

    plateCascade1.detectMultiScale(gray, plates1, 1.1, 10, 0, cv::Size(50, 50));
    
    plateCascade2.detectMultiScale(gray, plates2, 1.1, 10, 0, cv::Size(50, 50));

    plates1.insert(plates1.end(), plates2.begin(), plates2.end());

    if (plates1.empty()) {
        return false;  
    }

    plateRegion = plates1[0];
    return true;
}


bool ImageProcessor::isDuplicate(const std::string& plateNumber) {
    return licensePlates.find(plateNumber) != licensePlates.end();
}

std::string ImageProcessor::processLicensePlate(const cv::Mat& plateRegion) {
   
    return "ABC123";  
}