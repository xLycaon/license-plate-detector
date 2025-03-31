#ifndef FILEPARSER_HPP
#define FILEPARSER_HPP

#include <string>
#include <vector>
#include <opencv2/opencv.hpp>
#include "ImageProcessor.hpp"

class FileParser {
public:
    // Constructor & Destructor
    FileParser() = default;
    ~FileParser() = default;

    // Load images from a folder with their file names
    std::vector<std::pair<cv::Mat, std::string>> loadImagesFromFolder(const std::string& folderPath);

    // Process all images from a folder with ImageProcessor
    void processImagesInFolder(const std::string& folderPath, ImageProcessor& processor);

private:
    // Helper function to get all image files from the directory
    std::vector<std::string> getImageFilesFromDirectory(const std::string& folderPath);

    // Helper function to load an image from file
    cv::Mat loadImage(const std::string& filePath);
};

#endif // FILEPARSER_HPP
