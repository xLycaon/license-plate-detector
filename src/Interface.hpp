#pragma once

#include "FileParser.hpp"
#include "ImageProcessor.hpp"
#include <opencv2/opencv.hpp>

class Interface {
public:
    Interface();
    void run();

private:
    FileParser parser_;
    ImageProcessor processor_;

    cv::Mat menuImage_;
    const int buttonHeight_ = 100;
    const int buttonWidth_ = 400;
    const int buttonSpacing_ = 20;
    const int windowWidth_ = 500;
    const int windowHeight_;

    void drawMenu();
    static void onMouse(int event, int x, int y, int flags, void* userdata);

    void handleButtonClick(int x, int y);
    void onExit();
    void onAnalyzeFolder();
    void onAnalyzeSingleImage();
    void drawButton(const std::string& text, int y, const cv::Scalar& color);
    void updateDisplay();

    std::vector<std::string> getSubfolders(const std::string& path);
    std::vector<std::string> getImages(const std::string& path);
    int displayListAndSelect(const std::vector<std::string>& items, const std::string& title);
    void Interface::showImageCentered(cv::Mat& image);
    std::string basePath_; 
    std::string statusMessage_;
    const std::string windowName_ = "Menu";
};
