#include "VideoCapture.hpp"
#include <iostream>

VideoCapture::VideoCapture(int deviceID) : cap(deviceID), windowWidth(640), windowHeight(480), isTimerActive(false) {
    if (!cap.isOpened()) {
        std::cerr << "Error: Unable to access the camera!" << std::endl;
    }
}

VideoCapture::~VideoCapture() {
    cap.release(); 
    cv::destroyAllWindows(); 
}

void VideoCapture::setWindowSize(int width, int height) {
    windowWidth = width;
    windowHeight = height;
}

void VideoCapture::startTimer(int seconds) {
    startTime = std::chrono::high_resolution_clock::now();
    isTimerActive = true;
    std::cout << "Timer started for " << seconds << " seconds." << std::endl;
}

std::vector<cv::Mat> VideoCapture::getFrames() {
    std::vector<cv::Mat> frames;
    while (true) {
        cap >> currentFrame; 
        if (currentFrame.empty()) {
            std::cerr << "Error: Failed to capture frame!" << std::endl;
            break;
        }

        
        cv::resize(currentFrame, currentFrame, cv::Size(windowWidth, windowHeight));

       
        cv::imshow("Webcam Feed", currentFrame);

       
        frames.push_back(currentFrame);

       
        closeWindowIfTimeElapsed();

        if (cv::waitKey(1) == 27) {  //Esc-key to exit
            break;
        }
    }

    return frames;
}

void VideoCapture::closeWindowIfTimeElapsed() {
    if (isTimerActive) {
        auto currentTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(currentTime - startTime).count();
        if (duration >= 10) { 
            std::cout << "Timer expired! Closing window." << std::endl;
            cv::destroyWindow("Webcam Feed");
            isTimerActive = false;
        }
    }
}