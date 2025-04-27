#include "Interface.hpp"
#include <opencv2/opencv.hpp>
#include <filesystem>

namespace fs = std::filesystem;

std::string findTopLevelCMakeDirectory() {
    std::filesystem::path currentPath = std::filesystem::current_path();

    // Traverse upwards looking for the CMakeLists.txt file
    while (currentPath.has_parent_path()) {
        currentPath = currentPath.parent_path();  // Go one level up

        if (std::filesystem::exists(currentPath / "CMakeLists.txt")) {
            return currentPath.string();  // Found the top-level directory
        }
    }

    return "";  // Return empty string if not found
}

Interface::Interface()
    : windowHeight_((buttonHeight_ + buttonSpacing_) * 3 + buttonSpacing_ + 50),windowWidth_(1000) // Increased height for status message
{
    // Try to find the top-level CMake directory, otherwise fallback to './resources'
    basePath_ = findTopLevelCMakeDirectory();
    if (basePath_.empty()) {
        basePath_ = "./resources";  // Default path if CMakeLists.txt is not found
    }

    menuImage_ = cv::Mat::zeros(windowHeight_, windowWidth_, CV_8UC3);
    statusMessage_ = "Welcome!";  // Initial message
}

void Interface::run() {
    drawMenu();
    cv::imshow(windowName_, menuImage_);
    cv::setMouseCallback(windowName_, onMouse, this);

    while (true) {
        int key = cv::waitKey(1);
        if (key == 27) { // ESC key to exit
            break;
        }
    }
    cv::destroyAllWindows();
}

void Interface::showImageCentered(cv::Mat& image) {
    // Get the size of the image and the window
    int imageWidth = image.cols;
    int imageHeight = image.rows;

    // Resize the image if it's too large for the window (optional)
    int maxWidth = windowWidth_ - 40;  // Add some padding
    int maxHeight = windowHeight_ - 40;

    double aspectRatio = double(imageWidth) / double(imageHeight);
    int newWidth = imageWidth;
    int newHeight = imageHeight;

    // If the image is too wide or too tall, scale it down while maintaining aspect ratio
    if (imageWidth > maxWidth) {
        newWidth = maxWidth;
        newHeight = int(newWidth / aspectRatio);
    }
    if (newHeight > maxHeight) {
        newHeight = maxHeight;
        newWidth = int(newHeight * aspectRatio);
    }

    // Resize the image to fit the window
    cv::Mat resizedImage;
    cv::resize(image, resizedImage, cv::Size(newWidth, newHeight));

    // Create a blank canvas to draw the centered image
    cv::Mat canvas = cv::Mat::zeros(windowHeight_, windowWidth_, CV_8UC3);  // Blank canvas

    // Calculate the center position
    int xOffset = (windowWidth_ - newWidth) / 2;
    int yOffset = (windowHeight_ - newHeight) / 2;

    // Copy the resized image onto the canvas (centered)
    resizedImage.copyTo(canvas(cv::Rect(xOffset, yOffset, resizedImage.cols, resizedImage.rows)));

    // Show the image
    cv::imshow("Centered Image", canvas);
    cv::waitKey(1);
}

void Interface::drawMenu() {
    menuImage_.setTo(cv::Scalar(50, 50, 50)); // Dark background

    // Draw Buttons
    drawButton("Analyze Folder", 20, cv::Scalar(100, 100, 255));
    drawButton("Analyze Image", 140, cv::Scalar(100, 255, 100));
    drawButton("Exit", 260, cv::Scalar(255, 100, 100));

    // Draw Status Message
    int messageYPosition = windowHeight_ - 40;  // Adjust position to avoid overlapping buttons
    cv::putText(menuImage_, statusMessage_, { 20, messageYPosition },
        cv::FONT_HERSHEY_SIMPLEX, 0.8, cv::Scalar(255, 255, 255), 2);
}

void Interface::drawButton(const std::string& text, int y, const cv::Scalar& color) {
    cv::rectangle(menuImage_, { 50, y }, { 450, y + buttonHeight_ }, color, -1);
    cv::putText(menuImage_, text, { 100, y + 60 }, cv::FONT_HERSHEY_SIMPLEX, 1,
        cv::Scalar(0, 0, 0), 2);
}

void Interface::onMouse(int event, int x, int y, int flags, void* userdata) {
    if (event != cv::EVENT_LBUTTONDOWN) return;

    Interface* self = static_cast<Interface*>(userdata);
    if (self) {
        self->handleButtonClick(x, y);
    }
}

void Interface::handleButtonClick(int x, int y) {
    if (y >= 20 && y <= 20 + buttonHeight_) {
        onAnalyzeFolder();
    }
    else if (y >= 140 && y <= 140 + buttonHeight_) {
        onAnalyzeSingleImage();
    }
    else if (y >= 260 && y <= 260 + buttonHeight_) {
        onExit();
    }
}

void Interface::onExit() {
    statusMessage_ = "Exiting program...";  // Update the message
    updateDisplay();  // Redraw with the updated message
    cv::waitKey(1);
    cv::destroyAllWindows();
    exit(0);
}

void Interface::onAnalyzeFolder() {
    std::vector<std::string> folders = getSubfolders(basePath_);

    if (folders.empty()) {
        statusMessage_ = "No folders found!";
        updateDisplay();
        return;
    }

    int selected = displayListAndSelect(folders, "Select a Folder");

    if (selected >= 0 && selected < folders.size()) {
        std::string selectedPath = folders[selected];

        // Get images from the selected folder
        std::vector<std::string> images = getImages(selectedPath);

        if (images.empty()) {
            statusMessage_ = "No images found in folder!";
            updateDisplay();
            return;
        }

        // Process each image one by one
        for (const std::string& imagePath : images) {
            statusMessage_ = "Loading... " + fs::path(imagePath).filename().string(); // Show loading message
            updateDisplay();

            // Load the image
            cv::Mat image = parser_.loadImage(imagePath);

            if (image.empty()) {
                statusMessage_ = "Could not load image!";
                updateDisplay();
                continue;
            }

            // Display the image for 5 seconds
            showImageCentered(image);

            // Perform license plate detection and recognition here
            std::vector<cv::Rect> plateRegions1;
            std::vector<cv::Rect> plateRegions2;
            Image temp;

            temp.detectLicensePlate(image, plateRegions1);
            temp.detectLicensePlate(image, plateRegions2);

            std::string extractedText1 = processor_.process_Extract_Adaptive(image, plateRegions1);
            std::string extractedText2 = processor_.test_OCR_grayScale(image, plateRegions2);

            std::string result = processor_.findMostSimilarLicensePlate(extractedText1, extractedText2);
            statusMessage_ =  result;

            updateDisplay();

            // Wait for the user to press any key before continuing to the next image
            cv::waitKey(6000);
        }

        statusMessage_ = "Finished analyzing folder.";  // Once all images are processed
    }
    else {
        statusMessage_ = "No folder selected!";
    }

    updateDisplay();
}

void Interface::onAnalyzeSingleImage() {
    std::vector<std::string> images = getImages(basePath_);

    if (images.empty()) {
        statusMessage_ = "No images found!";  // No images found message
        updateDisplay();
        return;
    }

    int selected = displayListAndSelect(images, "Select an Image");

    if (selected >= 0 && selected < images.size()) {
        std::string selectedPath = images[selected];
        cv::Mat image = parser_.loadImage(selectedPath);

        if (image.empty()) {
            statusMessage_ = "Could not load image!";  // Image loading error message
            updateDisplay();
            return;
        }

		std::string name = fs::path(images[selected]).filename().string();
        statusMessage_ = "Loading " + name; // Show loading message
        updateDisplay();

        std::vector<cv::Rect> plateRegions1;
        std::vector<cv::Rect> plateRegions2;
        Image temp;

        showImageCentered(image);
        

        temp.detectLicensePlate(image, plateRegions1);
        temp.detectLicensePlate(image, plateRegions2);

        std::string extractedText1 = processor_.process_Extract_Adaptive(image, plateRegions1);
        std::string extractedText2 = processor_.test_OCR_grayScale(image, plateRegions2);

        std::string result = processor_.findMostSimilarLicensePlate(extractedText1, extractedText2);

        // Construct the full result message
        statusMessage_ = result;
    }
    else {
        statusMessage_ = "No image selected!";  // No image selected message
    }

    updateDisplay();
}

void Interface::updateDisplay() {
    drawMenu();  // Redraw the menu with the updated status message
    cv::imshow(windowName_, menuImage_);
}

std::vector<std::string> Interface::getSubfolders(const std::string& path) {
    std::vector<std::string> folders;
    for (const auto& entry : fs::directory_iterator(path)) {
        if (fs::is_directory(entry)) {
            folders.push_back(entry.path().string());
        }
    }
    return folders;
}

std::vector<std::string> Interface::getImages(const std::string& path) {
    std::vector<std::string> images;
    for (const auto& entry : fs::directory_iterator(path)) {
        if (fs::is_regular_file(entry)) {
            std::string ext = entry.path().extension().string();
            if (ext == ".jpg" || ext == ".jpeg" || ext == ".png" || ext == ".JPG") {
                images.push_back(entry.path().string());
            }
        }
    }
    return images;
}

int Interface::displayListAndSelect(const std::vector<std::string>& items, const std::string& title) {
    cv::Mat listWindow = cv::Mat::zeros(600, 800, CV_8UC3);
    listWindow.setTo(cv::Scalar(30, 30, 30));

    int y = 50;
    for (size_t i = 0; i < items.size(); ++i) {
        cv::putText(listWindow, std::to_string(i) + ": " + fs::path(items[i]).filename().string(),
            { 30, y }, cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(255, 255, 255), 2);
        y += 40;
    }

    cv::putText(listWindow, "Press the number key corresponding to your choice",
        { 30, y + 30 }, cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(100, 255, 100), 2);

    cv::imshow(title, listWindow);

    int key = -1;
    while (true) {
        key = cv::waitKey(0);
        if (key >= '0' && key <= '9') {
            int selectedIndex = key - '0';
            if (selectedIndex >= 0 && selectedIndex < items.size()) {
                cv::destroyWindow(title);
                return selectedIndex;
            }
        }
    }
}
