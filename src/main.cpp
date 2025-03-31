#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <string>
#include "FileParser.hpp"
#include "ImageProcessor.hpp"
#include "Image.hpp"


int main() {
    //deactivate debug log messages
    cv::utils::logging::setLogLevel(cv::utils::logging::LOG_LEVEL_ERROR);
    freopen("/dev/null", "w", stderr);
    std::cerr.setstate(std::ios_base::failbit);

    // Initialize FileParser and ImageProcessor
    FileParser parser;
    ImageProcessor processor;
    Image image1;

    // Folder containing the license plate images
    std::string folderPath = "C:\\Users\\vladm\\source\\repos\\license-plate-detector\\src\\images\\tested images";  // Set your folder path here


    // Process images in the folder and analyze the extracted text
    parser.processImagesInFolder(folderPath, processor);
    processor.showAverageConfidence(processor.averageConfidence, processor.numberOfFiles);
    // Example: Show each processed image with its text
    //auto images = parser.loadImagesFromFolder(folderPath);
    //for (const auto& [image, fileName] : images) {
    //    // Show each image in OpenCV window
    //    cv::imshow("License Plate Detection", image);
    //    cv::waitKey(1);
    //    std::vector<cv::Rect> plateRegions;
    //    image1.detectLicensePlate(image, plateRegions);
    //    std::vector<cv::Rect> plateRegions1;
    //    image1.detectLicensePlate(image, plateRegions1);
    //    // Extract text using Sobel and Adaptive methods
    //    std::string extractedText1 = processor.process_Extract_Adaptive(image, plateRegions);
    //    std::string temp = processor.test_OCR_grayScale(image, plateRegions1);
    //    // Analyze the text and calculate the accuracy for Adaptive
    //    std::string result1 = processor.findMostSimilarLicensePlate(extractedText1,temp);
    //    
    //}

    // Clean up OpenCV windows after processing
    cv::destroyAllWindows();

    return 0;
}