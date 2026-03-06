#pragma once

#include <vector>

#include <base/image.hpp>
#include <base/vec2d.hpp>
#include <base/color.hpp>

/**
 * @brief 保持比例的缩小图像尺寸直到其尺寸符合给定的尺寸限制。
 * @param img 三通道图像。
 * @note 如果所给图像是符合给定限制的则不做任何事。
 */
Image limitImageScale(const Image& img, int width, int height);

/**
 * @brief 计算图像指定位置的能量值。（能量值越高意味着此处细节越丰富，反之亦然）
 * @param img 三通道图像。
 * @return 图像指定位置的能量值，其值域为[0.0, 1.0]，能量值越高，能量值越接近1.0。
 */
double computePointEnergy(const Image& img, int row, int col,
    double rWeight = 1.0, double gWeight = 1.0, double bWeight = 1.0);

/**
 * @brief 将缩放后图像中的线条坐标映射回原始图像坐标系。
 * @param scaledLine 缩放后图像中的线条。
 * @param originalSize 原始图像的尺寸（rows, cols）。
 * @param scaledSize 缩放后图像的尺寸（rows, cols）。
 */
std::vector<IPos> mapLineToOriginalSize(
    const std::vector<IPos>& scaledLine,
    const ISize& originalSize,
    const ISize& scaledSize);

void highlightLine(Image& img, const std::vector<IPos>& line, const RgbColor& color);

Image removeLine(const Image& img, const std::vector<IPos>& line);
