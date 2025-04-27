# License Plate Detector 🚗🔍

## 📖 Overview

This project is a **License Plate Detection** and **Recognition** system that utilizes **OpenCV** to detect and recognize vehicle license plates in images. It is designed for **automatic number plate recognition (ANPR)** applications, which can be useful in scenarios like **traffic monitoring** or **vehicle identification**.

The project comes with a **Graphical User Interface (GUI)** to make the user experience easier and more intuitive.

## 🎯 Features

- **License Plate Detection**: Automatically detects license plates in various vehicle images.
- **Character Recognition**: Extracts and recognizes alphanumeric characters from detected license plates using custom recognition algorithms.
- **Multiple Image Formats**: Supports common image formats such as `.jpg`, `.jpeg`, and `.png`.
- **GUI Interface**: The project includes a GUI to display images and provide feedback on the detection process.
- **Easy to Set Up and Run**: Simple compilation process using **CMake**.

---

## 🛠️ Prerequisites

Before you begin, make sure you have the following installed:

- **C++ Compiler**: GCC (Linux), MSVC (Windows), or any other standard C++ compiler.
- **CMake**: For building the project from source.
- **OpenCV**: The core library used for image processing and manipulation.

### 🗂️ Required Libraries

All the necessary libraries are provided via a **Google Drive link**. You need to download and unzip them into the project folder.

- **Download Libraries**: [Google Drive Link](https://drive.google.com/drive/folders/1s5Qu4zuSXiyUdxlkJr-_1Ar1KTuXOsP5?usp=sharing)

---

## ⚙️ Installation

### 1. **Download Libraries**
- Click on the link above to download the required libraries.
- **Unzip** the downloaded contents into the root directory of this project.

### 2. **Set Up Your Environment**
- Install a **C++ compiler** appropriate for your system.
- Install **CMake** if you don’t have it already.

### 3. **Build the Project**
- **Open a terminal/command prompt** and navigate to the project folder.
- Create a **build directory** and move into it:
  ```bash
  mkdir build
  cd build
  ```
Run CMake to generate the necessary build files:

  ```bash
  cmake ..
  ```

Compile the Project:

  ```bash
  make
  ```
This will compile the project and create an executable in the build directory.
