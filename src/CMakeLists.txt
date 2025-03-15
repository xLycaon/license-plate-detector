cmake_minimum_required(VERSION 3.10)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Include OpenCV
set(OpenCV_DIR "${CMAKE_SOURCE_DIR}/opencv/build")
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})


# Add executable
add_executable(license-plate-detector main.cpp "ImageProcessor.hpp" "ImageProcessor.cpp"  "Interface.cpp" "VideoCapture.cpp" "Image.cpp")

# Link OpenCV and Qt libraries
target_link_libraries(license-plate-detector 
    ${OpenCV_LIBS} 
)
