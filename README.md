# 内容感知的图像裁切

**简体中文** | [English](README_EN.md)

[![C++ Standard](https://img.shields.io/badge/C%2B%2B-17-blue?logo=cplusplus)](https://isocpp.org/)
[![CMake](https://img.shields.io/badge/CMake-%3E%3D3.17-064F8C?logo=cmake)](https://cmake.org/)
[![Qt](https://img.shields.io/badge/Qt-5%20%7C%206-41CD52?logo=qt)](https://www.qt.io/)
[![Platform](https://img.shields.io/badge/Platform-Windows%20%7C%20Linux%20%7C%20macOS-lightgrey)](https://github.com/JaderoChan/ContentAwareImageCrop)
[![Version](https://img.shields.io/badge/Version-1.0.0-brightgreen)](https://github.com/JaderoChan/ContentAwareImageCrop/releases)

基于**接缝雕刻（Seam Carving）**算法的内容感知图像裁切工具。通过识别并移除图像中能量最低的像素路径（接缝），在缩减图像尺寸的同时最大限度地保留图像中的高细节区域。

> 初版：[Content Aware Image Resize](https://github.com/JaderoChan/Content-aware-image-resizing)
>
> 参考文章：[Real-world dynamic programming: seam carving](https://avikdas.com/2019/05/14/real-world-dynamic-programming-seam-carving.html)

---

## 目录

- [内容感知的图像裁切](#内容感知的图像裁切)
  - [目录](#目录)
  - [功能特性](#功能特性)
  - [算法原理](#算法原理)
  - [依赖要求](#依赖要求)
  - [构建方法](#构建方法)
    - [1. 克隆仓库](#1-克隆仓库)
    - [2. 配置与构建](#2-配置与构建)
    - [3. 构建测试程序（可选）](#3-构建测试程序可选)
  - [使用说明](#使用说明)
  - [项目结构](#项目结构)
  - [参考资料](#参考资料)

---

## 功能特性

- **内容感知裁切**：基于接缝雕刻算法，智能识别并优先移除图像低细节区域，保留视觉主体。
- **能量可视化**：支持将图像的能量分布转换为灰度图直观展示。
- **实时进度反馈**：裁切过程中逐步展示带高亮接缝的中间结果。
- **接缝高亮**：可自定义高亮颜色与抗锯齿选项，直观显示每次移除的像素路径。
- **撤销 / 重做**：支持多步操作历史，方便对比不同裁切结果。
- **图像尺寸限制**：裁切前可自动将大图缩放至指定尺寸，提升处理效率。
- **多语言界面**：内置中文与英文界面，可在运行时切换。
- **跨版本 Qt 支持**：同时兼容 Qt 5 与 Qt 6。

---

## 算法原理

接缝雕刻算法的核心步骤如下：

1. **能量计算**：对图像每个像素计算能量值（基于 RGB 各通道的梯度变化），能量越高表示该像素周围细节越丰富。
2. **接缝寻路**：使用动态规划在全图中找出从顶到底（或从左到右）能量总和最小的像素路径，即一条「接缝」。
3. **接缝移除**：将该接缝上的所有像素从图像中删除，图像宽度（或高度）减少 1 像素。
4. **重复执行**：重复上述步骤直到达到目标尺寸。

$$
E(i,j) = \left|\frac{\partial I}{\partial x}\right| + \left|\frac{\partial I}{\partial y}\right|
$$

---

## 依赖要求

| 依赖 | 版本要求 | 说明 |
|------|---------|------|
| CMake | ≥ 3.17 | 构建系统 |
| C++ 编译器 | C++17 | MSVC / GCC / Clang |
| Qt | 5 或 6 | Widgets 模块 |

第三方库（已包含在项目中，无需额外安装）：

| 库 | 说明 |
|----|------|
| [stb_image](https://github.com/nothings/stb) | 轻量图像读取库（后端） |
| [easy_translate](frontend/3rdparty/easy_translate) | 轻量 Qt 国际化库（前端） |

---

## 构建方法

### 1. 克隆仓库

```bash
git clone https://github.com/JaderoChan/ContentAwareImageCrop.git
cd ContentAwareImageCrop
```

### 2. 配置与构建

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

> **提示**：若系统中同时安装了 Qt 5 与 Qt 6，可通过 `-DCMAKE_PREFIX_PATH=<Qt安装路径>` 指定使用的 Qt 版本，例如：
> ```bash
> cmake -B build -DCMAKE_PREFIX_PATH="C:/Qt/6.x.x/msvc2019_64"
> ```

构建完成后，可执行文件输出至 `build/frontend/bin/Release/` 目录。

### 3. 构建测试程序（可选）

```bash
cmake -B build -DBUILD_TEST=ON
cmake --build build --config Release
```

---

## 使用说明

1. 启动 `ContentAwareImageCropFrontend.exe`。
2. 通过菜单或拖放打开一张图像。
3. 设置裁切参数：
   - **裁切量**：移除的接缝数量，即减少的像素宽度（或高度）。
   - **尺寸限制**：勾选后，算法处理前会先将图像缩放至指定尺寸（可提升速度）。
   - **高亮颜色**：设置接缝可视化的颜色。
   - **抗锯齿**：对接缝高亮线启用抗锯齿。
4. 点击「裁切」按钮开始处理，实时查看进度。
5. 使用撤销 / 重做按钮对比不同步骤的结果。
6. 导出裁切后的图像。

---

## 项目结构

```
ContentAwareImageCrop/
├── backend/                  # 后端核心算法库（静态库）
│   ├── src/
│   │   ├── base/             # 基础数据类型（Image、Mat、Color 等）
│   │   ├── energy_mat.hpp/cpp    # 能量图计算
│   │   ├── color_similarity.hpp  # 颜色相似度计算
│   │   └── utilities.hpp/cpp     # 图像工具函数（缩放、接缝移除等）
│   └── test/                 # 后端测试程序
├── frontend/                 # Qt GUI 前端
│   ├── src/
│   │   └── app/              # 主窗口、裁切工作线程、多语言等
│   ├── languages/            # 语言文件（zh.json、en.json）
│   ├── resources/            # 资源文件（图标等）
│   └── 3rdparty/
│       └── easy_translate/   # 国际化第三方库
└── CMakeLists.txt            # 顶层 CMake 配置
```

---

## 参考资料

- Avidan, S., & Shamir, A. (2007). *Seam carving for content-aware image resizing*. ACM SIGGRAPH 2007.
- [Real-world dynamic programming: seam carving](https://avikdas.com/2019/05/14/real-world-dynamic-programming-seam-carving.html) — Avik Das
- 初版实现：[Content Aware Image Resize](https://github.com/JaderoChan/Content-aware-image-resizing)
