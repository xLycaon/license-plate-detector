cmake_minimum_required(VERSION 3.10)

# Set policy to allow linking libraries that are not built in this directory
cmake_policy(SET CMP0079 NEW)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Include OpenCV
set(OpenCV_DIR "${CMAKE_SOURCE_DIR}/opencv/build")
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

# Include Tesseract and Leptonica
set(TESSERACT_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/tesseract/include")
set(LEPTONICA_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/tesseract/include")

set(TESSERACT_LIB_DIR "${CMAKE_SOURCE_DIR}/tesseract/lib")
set(LEPTONICA_LIB_DIR "${CMAKE_SOURCE_DIR}/tesseract/lib")

include_directories("${TESSERACT_INCLUDE_DIR}")
include_directories("${LEPTONICA_INCLUDE_DIR}")

link_directories(${TESSERACT_LIB_DIR})
link_directories(${LEPTONICA_LIB_DIR})

# Set Haar Cascade path
set(HAAARCASCADES_PATH "${CMAKE_SOURCE_DIR}/opencv/build/etc/haarcascades")
set(TESSDATA_PREFIX "${CMAKE_SOURCE_DIR}/tesseract/tessdata/")
# Add subdirectory for src
add_subdirectory(src)
