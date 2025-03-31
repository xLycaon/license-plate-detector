#include "FileParser.hpp"
#include <opencv2/imgcodecs.hpp>
#include <filesystem>
#include <iostream>
#include <string>

namespace fs = std::filesystem;

// Load images from the folder along with their file names
std::vector<std::pair<cv::Mat, std::string>> FileParser::loadImagesFromFolder(const std::string& folderPath) {
    std::vector<std::pair<cv::Mat, std::string>> images;
    auto imageFiles = getImageFilesFromDirectory(folderPath);

    for (const auto& filePath : imageFiles) {
        cv::Mat image = loadImage(filePath);
        if (!image.empty()) {
            images.push_back({ image, filePath });  // Store image and its file name
        }
    }

    return images;
}

// Process all images from a folder using the ImageProcessor class
void FileParser::processImagesInFolder(const std::string& folderPath, ImageProcessor& processor) {
    auto images = loadImagesFromFolder(folderPath);

    for (const auto& [image, fileName] : images) {  // Accessing image and file name as a pair
        
        cv::Mat image1 = image;
        Image temp;
        std::vector<cv::Rect> plateRegions1;
        std::vector<cv::Rect> plateRegions2;

        //Show the picture that's being processed
        cv::imshow(fs::path(fileName).filename().string(), image);
        cv::waitKey(1);

        //Detect the regions
        temp.detectLicensePlate(image, plateRegions1);
        temp.detectLicensePlate(image1, plateRegions2);

        //Extract the text
        std::string extractedText1 = processor.process_Extract_Adaptive(image, plateRegions1);
        std::string extractedText2 = processor.test_OCR_grayScale(image1, plateRegions2);

        // Analyze the text
        std::string result = processor.findMostSimilarLicensePlate(extractedText1, extractedText2);

        cv::destroyWindow(fs::path(fileName).filename().string());
    }
}

// Get all image files in the directory
std::vector<std::string> FileParser::getImageFilesFromDirectory(const std::string& folderPath) {
    std::vector<std::string> imageFiles;

    for (const auto& entry : fs::directory_iterator(folderPath)) {
        if (fs::is_regular_file(entry) && (entry.path().extension() == ".jpg" || entry.path().extension() == ".png" || entry.path().extension() == ".JPG" || entry.path().extension() == ".jpeg")) {  // You can adjust file type based on needs
            imageFiles.push_back(entry.path().string());
        }
    }

    return imageFiles;
}

// Load an image from the given file path
cv::Mat FileParser::loadImage(const std::string& filePath) {
    cv::Mat image = cv::imread(filePath);
    if (image.empty()) {
        std::cerr << "Error loading image: " << filePath << std::endl;
    }
    return image;
}
