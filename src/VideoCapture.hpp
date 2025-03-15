#ifndef VIDEO_CAPTURE_HPP
#define VIDEO_CAPTURE_HPP

#include <opencv2/opencv.hpp>
#include <vector>
#include <chrono>

class VideoCapture {
public:
    VideoCapture(int deviceID = 0);
    ~VideoCapture(); 

    
    void setWindowSize(int width, int height);

    
    void startTimer(int seconds);

    
    std::vector<cv::Mat> getFrames();

private:
    cv::VideoCapture cap;  
    cv::Mat currentFrame;  
    int windowWidth;
    int windowHeight;

    bool isTimerActive;
    std::chrono::time_point<std::chrono::high_resolution_clock> startTime;

    void closeWindowIfTimeElapsed();
};

#endif
