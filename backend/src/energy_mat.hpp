#pragma once

#include <base/pos.hpp>
#include <base/image.hpp>

/**
 * @brief 计算图像指定位置的能量值。（能量值越高意味着此处细节越丰富，反之亦然）
 * @param img 三通道图像。
 * @return 图像指定位置的能量值，其值域为 `[0.0, 1.0]`，能量值越高，能量值越接近 `1.0`。
 */
double computePointEnergy(const Image& img, int row, int col,
    double rWeight = 1.0, double gWeight = 1.0, double bWeight = 1.0);

/**
 * @brief 创建给定图像的能量图。
 */
Mat<double> createEnergyMat(const Image& img);

/**
 * @brief 重映射能量图的能量值范围至 `[0.0, 1.0]`，其中 `1.0` 对应原能量图中的最大能量值。
 */
Mat<double> normalizeEnergyMat(const Mat<double>& energyMat);

/**
 * @brief 将能量图转换成灰度图。
 */
Image energyMatToGrayImage(const Mat<double>& energyMat);

/**
 * @brief 取得给定图像的最小能量线。
 */
std::vector<IPos> fetchMinimumEnergyLine(const Image& img);
