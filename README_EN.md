# Content-Aware Image Crop

[简体中文](README.md) | **English**

[![C++ Standard](https://img.shields.io/badge/C%2B%2B-17-blue?logo=cplusplus)](https://isocpp.org/)
[![CMake](https://img.shields.io/badge/CMake-%3E%3D3.17-064F8C?logo=cmake)](https://cmake.org/)
[![Qt](https://img.shields.io/badge/Qt-5%20%7C%206-41CD52?logo=qt)](https://www.qt.io/)
[![Platform](https://img.shields.io/badge/Platform-Windows%20%7C%20Linux%20%7C%20macOS-lightgrey)](https://github.com/JaderoChan/ContentAwareImageCrop)
[![Version](https://img.shields.io/badge/Version-1.0.0-brightgreen)](https://github.com/JaderoChan/ContentAwareImageCrop/releases)

A content-aware image cropping tool based on the **Seam Carving** algorithm. It intelligently detects and removes the lowest-energy pixel paths (seams) from an image, reducing its dimensions while preserving the most visually significant regions.

> First version: [Content Aware Image Resize](https://github.com/JaderoChan/Content-aware-image-resizing)
>
> Reference: [Real-world dynamic programming: seam carving](https://avikdas.com/2019/05/14/real-world-dynamic-programming-seam-carving.html)

---

## Table of Contents

- [Content-Aware Image Crop](#content-aware-image-crop)
  - [Table of Contents](#table-of-contents)
  - [Features](#features)
  - [Algorithm](#algorithm)
  - [Requirements](#requirements)
  - [Building](#building)
    - [1. Clone the repository](#1-clone-the-repository)
    - [2. Configure and build](#2-configure-and-build)
    - [3. Build the test program (optional)](#3-build-the-test-program-optional)
  - [Usage](#usage)
  - [Project Structure](#project-structure)
  - [References](#references)

---

## Features

- **Content-aware cropping**: Uses the Seam Carving algorithm to intelligently remove low-detail areas while preserving the visual subject.
- **Energy visualization**: Converts the energy distribution of an image into a grayscale map for inspection.
- **Real-time progress feedback**: Displays intermediate results with highlighted seams during the cropping process.
- **Seam highlighting**: Customizable highlight color and antialiasing options for visualizing each removed seam.
- **Undo / Redo**: Multi-step operation history for comparing cropping results.
- **Image size limit**: Optionally downscales large images to a specified size before processing to improve performance.
- **Multilingual UI**: Built-in Chinese and English interfaces, switchable at runtime.
- **Cross-version Qt support**: Compatible with both Qt 5 and Qt 6.

---

## Algorithm

The core steps of the Seam Carving algorithm are as follows:

1. **Energy computation**: Calculate an energy value for each pixel based on the gradient magnitude across RGB channels. Higher energy indicates richer local detail.
2. **Seam finding**: Use dynamic programming to find the minimum-energy path of pixels from top to bottom (or left to right), called a "seam".
3. **Seam removal**: Delete all pixels along the seam, reducing the image width (or height) by one pixel.
4. **Iteration**: Repeat until the target dimensions are reached.

$$
E(i,j) = \left|\frac{\partial I}{\partial x}\right| + \left|\frac{\partial I}{\partial y}\right|
$$

---

## Requirements

| Dependency | Version | Notes |
| ---------- | ------- | ----- |
| CMake | ≥ 3.17 | Build system |
| C++ Compiler | C++17 | MSVC / GCC / Clang |
| Qt | 5 or 6 | Widgets module required |

Third-party libraries (already bundled, no separate installation needed):

| Library | Description |
| ------- | ----------- |
| [stb_image](https://github.com/nothings/stb) | Lightweight image loading (backend) |
| [easy_translate](frontend/3rdparty/easy_translate) | Lightweight i18n library (frontend) |

---

## Building

### 1. Clone the repository

```bash
git clone https://github.com/JaderoChan/ContentAwareImageCrop.git
cd ContentAwareImageCrop
```

### 2. Configure and build

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

> **Tip**: If both Qt 5 and Qt 6 are installed, specify the desired version via `-DCMAKE_PREFIX_PATH`, e.g.:
>
> ```bash
> cmake -B build -DCMAKE_PREFIX_PATH="C:/Qt/6.x.x/msvc2019_64"
> ```

The executable is output to `build/frontend/bin/Release/` after a successful build.

### 3. Build the test program (optional)

```bash
cmake -B build -DBUILD_TEST=ON
cmake --build build --config Release
```

---

## Usage

1. Launch `ContentAwareImageCropFrontend.exe`.
2. Open an image via the menu or drag-and-drop.
3. Configure the cropping parameters:
   - **Crop amount**: Number of seams to remove, i.e., pixels to trim from the width (or height).
   - **Size limit**: When enabled, the image is pre-scaled to the specified size before processing (improves speed on large images).
   - **Highlight color**: Color used to visualize each removed seam.
   - **Antialiasing**: Enable antialiasing on the seam highlight overlay.
4. Click **Crop** to start processing and watch the real-time progress.
5. Use the undo / redo buttons to compare results at different stages.
6. Export the cropped image.

---

## Project Structure

```plaintext
ContentAwareImageCrop/
├── backend/                        # Core algorithm library (static library)
│   ├── src/
│   │   ├── base/                   # Fundamental data types (Image, Mat, Color, etc.)
│   │   ├── energy_mat.hpp/cpp      # Energy map computation
│   │   ├── color_similarity.hpp    # Color similarity utilities
│   │   └── utilities.hpp/cpp       # Image helpers (scaling, seam removal, etc.)
│   └── test/                       # Backend test program
├── frontend/                       # Qt GUI frontend
│   ├── src/
│   │   └── app/                    # Main window, crop worker thread, language support, etc.
│   ├── languages/                  # Language files (zh.json, en.json)
│   ├── resources/                  # Resource files (icons, etc.)
│   └── 3rdparty/
│       └── easy_translate/         # Third-party i18n library
└── CMakeLists.txt                  # Top-level CMake configuration
```

---

## References

- Avidan, S., & Shamir, A. (2007). *Seam carving for content-aware image resizing*. ACM SIGGRAPH 2007.
- [Real-world dynamic programming: seam carving](https://avikdas.com/2019/05/14/real-world-dynamic-programming-seam-carving.html) — Avik Das
- First version: [Content Aware Image Resize](https://github.com/JaderoChan/Content-aware-image-resizing)
