#pragma once

#include <vector>

#include <base/color.hpp>
#include <base/image.hpp>
#include <base/pos.hpp>

/**
 * @brief 保持比例的缩小图像尺寸直到其尺寸符合给定的尺寸限制。
 * @param img 三通道图像。
 * @note 如果指定图像符合给定限制则不做任何事。
 */
Image limitImageScale(const Image& img, int width, int height);

/**
 * @brief 将缩放后图像中的线条坐标映射回原始图像坐标。
 * @param scaledLine 缩放后图像中的线条。
 */
std::vector<IPos> mapLineToOriginalSize(
    const std::vector<IPos>& scaledLine,
    int originalRows, int originalCols,
    int scaledRows, int scaledCols);

/**
 * @brief 以指定颜色对图像的给定线条区域着色。
 */
Image highlightLine(const Image& img, const std::vector<IPos>& line, const RgbColor& color);

/**
 * @brief 移除图像的指定线条区域。
 */
Image removeLine(const Image& img, const std::vector<IPos>& line);
