# Add executable
add_executable(license-plate-detector  "ImageProcessor.hpp" "ImageProcessor.cpp" "Interface.cpp"  "Image.cpp" "main.cpp" "FileParser.cpp" "Map.hpp")

# Link OpenCV, Tesseract, and Leptonica libraries
target_link_libraries(license-plate-detector 
    ${OpenCV_LIBS} 
    tesseract55
    leptonica-1.85.0
)

# Define HaarCascade path
target_compile_definitions(license-plate-detector PRIVATE HAAARCASCADES_PATH="${CMAKE_SOURCE_DIR}/opencv/build/etc/haarcascades")
target_compile_definitions(license-plate-detector PRIVATE TESSDATA_PREFIX="${CMAKE_SOURCE_DIR}/tesseract/tessdata")