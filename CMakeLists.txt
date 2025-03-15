cmake_minimum_required(VERSION 3.10)

# Set project name
project(license-plate-detector)

# Set C++ Standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set OpenCV directory (adjust path if needed)
set(OpenCV_DIR "${CMAKE_SOURCE_DIR}/opencv/build")
find_package(OpenCV REQUIRED)

set(HAAARCASCADES_PATH "${CMAKE_SOURCE_DIR}/opencv/build/etc/haarcascades")


add_subdirectory(src)

target_compile_definitions(license-plate-detector PRIVATE HAAARCASCADES_PATH="${HAAARCASCADES_PATH}")
cmake_policy(SET CMP0079 NEW)
target_link_libraries(license-plate-detector
    ${OpenCV_LIBS}
)