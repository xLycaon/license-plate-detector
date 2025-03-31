#include "ImageProcessor.hpp"
#include <opencv2/opencv.hpp>
#include <vector>
#include <string>
#include <queue>
#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <tesseract/baseapi.h>
#include <cmath>
#include <filesystem>
#include <algorithm>
#include <regex>
#include "Map.hpp"

// Constructor & Destructor
ImageProcessor::ImageProcessor() {
    
    
}
ImageProcessor::~ImageProcessor() {}

double ImageProcessor::averageConfidence = 0.0;
int ImageProcessor::numberOfFiles = 0;

//------------------------------Helper Functions---------------------------------//

// Compute Levenshtein distance between two strings
double levenshteinSimilarity(const std::string& s1, const std::string& s2) {
    int len1 = s1.size(), len2 = s2.size();
    std::vector<std::vector<int>> dp(len1 + 1, std::vector<int>(len2 + 1));

    for (int i = 0; i <= len1; i++) {
        for (int j = 0; j <= len2; j++) {
            if (i == 0)
                dp[i][j] = j; // Deletion
            else if (j == 0)
                dp[i][j] = i; // Insertion
            else if (s1[i - 1] == s2[j - 1])
                dp[i][j] = dp[i - 1][j - 1]; // No operation
            else
                dp[i][j] = 1 + std::min({ dp[i - 1][j], dp[i][j - 1], dp[i - 1][j - 1] }); // Insert/Delete/Replace
        }
    }

    int maxLen = std::max(len1, len2);
    return 1.0 - (static_cast<double>(dp[len1][len2]) / maxLen); // Normalize to 0-1 range
}

// Compute accuracy percentage based on Levenshtein distance
double computeAccuracy(const std::string& correct, const std::string& detected) {
    int levDist = levenshteinSimilarity(correct, detected);
    return (1.0 - static_cast<double>(levDist) / std::max(correct.length(), detected.length())) * 100;
}

// Extract image name without extension
std::string getImageNameWithoutExtension(const std::string& imagePath) {
    std::filesystem::path path(imagePath);
    return path.stem().string();
}

// Remove non-alphanumeric characters from a string
void cleanText(std::string& text) {
    // Example of cleaning: remove non-alphanumeric characters
    text.erase(std::remove_if(text.begin(), text.end(), [](char c) {
        return !std::isalnum(c) && c != '\n';  // Retain alphanumeric characters and newlines
        }), text.end());
}

//---------------------------------------------------------------------------------//



//------------------------------Image Processing Function--------------------------//


//this function will convert to grayscale and try to apply OCR
std::string ImageProcessor::test_OCR_grayScale(const cv::Mat& frame, std::vector<cv::Rect>& plateRegions) {
    std::string extractedText;

    std::string whitelist = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    tesseract::TessBaseAPI tess;
    if (tess.Init("C:\\Users\\vladm\\source\\repos\\license-plate-detector\\tesseract\\tessdata", "eng",tesseract::OEM_TESSERACT_LSTM_COMBINED)) {
        std::cerr << "Could not initialize Tesseract." << std::endl;
        return "TEST";
    }

    tess.SetVariable("tessedit_char_whitelist", whitelist.c_str());
    tess.SetPageSegMode(tesseract::PSM_SINGLE_BLOCK);
    tess.SetVariable("tessedit_adapt_to_char_fragments", "0");

    
    for (auto& region : plateRegions) {
        cv::Mat plateRegionMat = frame(region);
        cv::Mat gray,thresholded;


        cv::cvtColor(plateRegionMat, gray, cv::COLOR_BGR2GRAY);

        tess.SetImage(gray.data, gray.cols, gray.rows, 1, gray.step);
        std::string text = tess.GetUTF8Text();

        text.erase(std::remove(text.begin(), text.end(), '\n'), text.end());
        text.erase(std::remove(text.begin(), text.end(), '\r'), text.end());
        text.erase(std::remove(text.begin(), text.end(), '\t'), text.end());
        text.erase(std::remove(text.begin(), text.end(), ' '), text.end());
        if (text.empty())continue;
        extractedText += text;
        extractedText += '\n';
        

    }
    return extractedText;
}



//Extract text from plate regions using adaptive thresholding
std::string ImageProcessor::process_Extract_Adaptive(const cv::Mat& frame, std::vector<cv::Rect>& plateRegions) {
    std::string extractedText;

    std::string whitelist = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

    unsigned int numThreads = std::min(4U, std::thread::hardware_concurrency()); // Max 4 threads
    std::queue<cv::Rect> taskQueue;
    std::mutex queueMutex;
    std::condition_variable condition;
    bool done = false;

    for (const auto& plateRegion : plateRegions) {
        taskQueue.push(plateRegion);
    }

    auto worker = [&]() {
        tesseract::TessBaseAPI localTess;
        if (localTess.Init("C:\\Users\\vladm\\source\\repos\\license-plate-detector\\tesseract\\tessdata", "eng", tesseract::OEM_TESSERACT_LSTM_COMBINED)) {
            std::cerr << "Could not initialize Tesseract." << std::endl;
            return;
        }

        localTess.SetVariable("tessedit_char_whitelist", whitelist.c_str());
        localTess.SetPageSegMode(tesseract::PSM_SINGLE_BLOCK);
        localTess.SetVariable("tessedit_adapt_to_char_fragments", "0");

        while (true) {
            cv::Rect plateRegion;
            {
                std::unique_lock<std::mutex> lock(queueMutex);
                condition.wait(lock, [&]() { return !taskQueue.empty() || done; });
                if (taskQueue.empty()) return;
                plateRegion = taskQueue.front();
                taskQueue.pop();
            }

            if (plateRegion.empty()) continue;

            cv::Mat plateRegionMat = frame(plateRegion);
            cv::Mat gray, enhanced, tophat, filtered, thresholded, resized;
            std::vector<int> blockSizes = { 13, 15, 17, 19 };
            std::vector<int> constants = { 5, 7, 9 };
            std::vector<double> scaleFactors = { 1, 1.5, 2.0, 2.5, 3.0 };
            std::vector<int> thresholdTypes = {cv::THRESH_BINARY,cv::THRESH_BINARY_INV};

            cv::cvtColor(plateRegionMat, gray, cv::COLOR_BGR2GRAY);
            cv::morphologyEx(gray, tophat, cv::MORPH_TOPHAT, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(15, 15)));
            gray.convertTo(enhanced, -1, 1.5, 30);
            cv::bilateralFilter(enhanced, filtered, 9, 75, 75);

            for (double scaleFactor : scaleFactors) {
                int newWidth = static_cast<int>(filtered.cols * scaleFactor);
                int newHeight = static_cast<int>(filtered.rows * scaleFactor);

                if (newWidth < 3 || newHeight < 3) continue;
                cv::resize(filtered, resized, cv::Size(newWidth, newHeight), 0, 0, cv::INTER_LINEAR);

                for (int blockSize : blockSizes) {
                    for (int C : constants) {
                        if (C >= blockSize) continue;
                        for (int thresholdType : thresholdTypes) {
                            cv::adaptiveThreshold(resized, thresholded, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C, thresholdType, blockSize, C);
                        
                            localTess.SetImage(thresholded.data, thresholded.cols, thresholded.rows, 1, thresholded.step);
                            std::string text = localTess.GetUTF8Text();

                            text.erase(std::remove(text.begin(), text.end(), '\n'), text.end());
                            text.erase(std::remove(text.begin(), text.end(), '\r'), text.end());
                            text.erase(std::remove(text.begin(), text.end(), '\t'), text.end());
                            text.erase(std::remove(text.begin(), text.end(), ' '), text.end());

                            {
                                std::lock_guard<std::mutex> lock(queueMutex);
                                if (text.length() >= 5 && text.length() <= 8) {
                                    extractedText += text;
                                    extractedText += '\n';
                                }
                            }
                        }
                    }
                }
            }
        }
    };

    std::vector<std::thread> workers;
    for (unsigned int i = 0; i < numThreads; ++i) {
        workers.emplace_back(worker);
    }

    {
        std::lock_guard<std::mutex> lock(queueMutex);
        done = true;
    }
    condition.notify_all();

    for (auto& worker : workers) {
        worker.join();
    }

    return extractedText;
}


//---------------------------------------------------------------------------------//


bool compareWithPattern(const std::string& substring, const std::string& pattern) {
    // Check if the substring matches the pattern using regex
    std::regex re(pattern);
    return std::regex_match(substring, re);
}

std::vector<std::string> splitTextIntoSubstrings(const std::string& detectedText) {
    std::vector<std::string> substrings;
    std::string temp = "";

    for (char c : detectedText) {
        if (c == ' ' || c == '\n' || c == '\t' || c == '\r') {
            substrings.push_back(temp);
            temp = "";
            continue;
        }
        temp += c;
    }

    return substrings;
}

double calculateConfidenceScore(const std::string& substring, int frequency, const std::string& detectedText) {
    int maxFrequency = 0;
    for (const auto& entry : splitTextIntoSubstrings(detectedText)) {
        maxFrequency = std::max(maxFrequency, frequency);
    }

    double frequencyWeight = maxFrequency > 0 ? (static_cast<double>(frequency) / maxFrequency) : 0;
    double lengthFactor = static_cast<double>(substring.size()) / detectedText.size();

    return 0.8 * frequencyWeight + 0.2 * lengthFactor;  // Weighted confidence score
}

bool containsOnlyNewlines(const std::string& str) {
    for (char c : str) {
        if (c != '\n' && c != '\r') {
            return false;  // Found a character that's not a newline or carriage return
        }
    }
    return true;  // All characters are newlines or carriage returns
}


// Find the most similar license plate based on OCR results
std::string ImageProcessor::findMostSimilarLicensePlate(const std::string& detectedText, const std::string& detectedText1) {
    std::vector<std::string> substrings = splitTextIntoSubstrings(detectedText);
    std::unordered_map<std::string, int> frequency;

    for (const auto& entry : substrings) {
        frequency[entry]++;
    }

    std::vector<std::pair<std::string, int>> frequencyList(frequency.begin(), frequency.end());
    std::sort(frequencyList.begin(), frequencyList.end(),
        [](const std::pair<std::string, int>& a, const std::pair<std::string, int>& b) {
            return a.second > b.second;
        });

    std::string bestSubstring;
    std::string bestCountry;
    double bestConfidence = 0.0;

    // If detectedText1 is empty, find the best match from detectedText
    if (containsOnlyNewlines(detectedText1)) {
        for (const auto& entry : frequencyList) {
            std::string substring = entry.first;
            int frequencyCount = entry.second;

            // Try correcting the substring by removing 1 extra leading/trailing character
            if (substring.size() > 2) {
                std::string trimmedLeading = substring.substr(1);  // Remove first character
                std::string trimmedTrailing = substring.substr(0, substring.size() - 1); // Remove last character

                if (frequency.count(trimmedLeading)) {
                    substring = trimmedLeading;
                }
                else if (frequency.count(trimmedTrailing)) {
                    substring = trimmedTrailing;
                }
            }

            for (const auto& countryPattern : countryLicensePatterns) {
                const std::string& country = countryPattern.first;
                const std::vector<std::string>& patterns = countryPattern.second;

                for (const std::string& pattern : patterns) {
                    try {
                        std::regex re(pattern);
                        if (std::regex_search(substring, re)) {
                            double confidence = calculateConfidenceScore(substring, frequencyCount, detectedText);

                            if (pattern.find(substring.substr(0, 2)) != std::string::npos) {
                                confidence += 0.5;  // Boost if region code matches
                            }

                            if (confidence > bestConfidence) {
                                bestSubstring = substring;
                                bestCountry = country;
                                bestConfidence = confidence;
                            }
                        }
                    }
                    catch (const std::regex_error& e) {
                        std::cerr << "Regex error: " << e.what() << " for pattern: " << country << std::endl;
                    }
                }
            }
        }
    }
    // Compare detectedText1 with detectedText
    else {
        std::vector<std::string> detectedText1Substrings = splitTextIntoSubstrings(detectedText1);
        std::unordered_map<std::string, double> confidenceScores;

        for (const std::string& substring : detectedText1Substrings) {
            if (substring.empty()) continue;

            int frequencyCount = frequency[substring];
            double confidence = calculateConfidenceScore(substring, frequencyCount, detectedText);

            if (frequencyCount > 0) {
                confidenceScores[substring] = confidence;
            }
            else {
                //  1. Check if substring is present in full detectedText
                if (detectedText.find(substring) != std::string::npos) {
                    confidenceScores[substring] = calculateConfidenceScore(substring, 1, detectedText) * 1.3;
                    continue;  // Skip further checks if we found it
                }

                //  2. Check Levenshtein similarity against detectedText
                for (const auto& entry : frequencyList) {
                    double similarity = levenshteinSimilarity(substring, entry.first);
                    if (similarity > 0.6) {  // Increase threshold slightly for more precision
                        double computedConfidence = calculateConfidenceScore(entry.first, entry.second, detectedText) * similarity;
                        confidenceScores[entry.first] = computedConfidence;
                    }
                }

                //  3. Try trimming leading/trailing characters if no match found
                std::vector<std::string> candidates;
                if (substring.size() > 1) candidates.push_back(substring.substr(1));
                if (substring.size() > 1) candidates.push_back(substring.substr(0, substring.size() - 1));
                if (substring.size() > 2) candidates.push_back(substring.substr(1, substring.size() - 2));

                for (const auto& candidate : candidates) {
                    if (frequency.count(candidate)) {
                        confidenceScores[candidate] = calculateConfidenceScore(candidate, frequency[candidate], detectedText) * 1.2;
                    }
                }
            }
        }

        for (const auto& entry : confidenceScores) {
            std::string substring = entry.first;
            double confidence = entry.second;

            // Check the substring with the first character removed
            if (substring.size() > 1 && std::isdigit(substring[0])) {
                std::string trimmedSubstring = substring.substr(1);  // Remove leading digit
                for (const auto& countryPattern : countryLicensePatterns) {
                    const std::string& country = countryPattern.first;
                    const std::vector<std::string>& patterns = countryPattern.second;

                    for (const std::string& pattern : patterns) {
                        try {
                            std::regex re(pattern);
                            if (std::regex_search(trimmedSubstring, re)) {
                                confidence += 0.5;  // Boost confidence for region code match

                                // If this trimmed version is a better match, update
                                if (confidence > bestConfidence) {
                                    bestSubstring = trimmedSubstring;
                                    bestCountry = country;
                                    bestConfidence = confidence;
                                }
                            }
                        }
                        catch (const std::regex_error& e) {
                            std::cerr << "Regex error: " << e.what() << " for pattern: " << country << std::endl;
                        }
                    }
                }
            }

            // Check the original substring as well
            for (const auto& countryPattern : countryLicensePatterns) {
                const std::string& country = countryPattern.first;
                const std::vector<std::string>& patterns = countryPattern.second;

                for (const std::string& pattern : patterns) {
                    try {
                        std::regex re(pattern);
                        if (std::regex_search(substring, re)) {
                            if (pattern.find(substring.substr(0, 2)) != std::string::npos) {
                                confidence += 0.5;  // Boost confidence for region code match
                            }

                            // If this match has the highest confidence so far, update
                            if (confidence > bestConfidence) {
                                bestSubstring = substring;
                                bestCountry = country;
                                bestConfidence = confidence;
                            }
                        }
                    }
                    catch (const std::regex_error& e) {
                        std::cerr << "Regex error: " << e.what() << " for pattern: " << country << std::endl;
                    }
                }
            }
        }
    }

    if (bestSubstring.empty()) {
        std::cerr << "No valid license plate match found." << std::endl;
        return "";
    }

    // Confidence should not exceed 100%
    bestConfidence = std::min(bestConfidence, 1.0);

    std::cout << "Final match: " << bestSubstring << " in country: " << bestCountry
        << " with confidence: " << static_cast<int>(bestConfidence * 100) << "%" << std::endl;

    averageConfidence += (bestConfidence * 100);
    numberOfFiles++;
    return bestSubstring;
}

void ImageProcessor::showAverageConfidence(double confidence, int numberOfPictures) {
    std::cout << "Average confidence of the program is: " << static_cast<int>(averageConfidence) / numberOfFiles <<"%";
}

