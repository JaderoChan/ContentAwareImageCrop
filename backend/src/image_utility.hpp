#pragma once

#include <vector>

#include <base/image.hpp>

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
