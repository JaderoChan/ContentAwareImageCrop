#pragma once

#include "color.hpp"

template <typename T>
constexpr T square(T x) { return x * x; }

constexpr int MAX_SQUARE = square<int>(UINT8_MAX);

/**
 * @brief 计算两种颜色的相似度。
 * @param rWeight RGB色彩空间中，红色分量的距离权重。
 * @param gWeight RGB色彩空间中，绿色分量的距离权重。
 * @param bWeight RGB色彩空间中，蓝色分量的距离权重。
 * @return 两种颜色的相似度，其值域位于[0.0, 1.0]。两种颜色越接近，相似度越接近1.0。
 */
constexpr double computeColorSimilarity(RgbColor lhs, RgbColor rhs,
    double rWeight = 1.0, double gWeight = 1.0, double bWeight = 1.0)
{
    return 1.0 - (
        square(lhs.r - rhs.r) * rWeight +
        square(lhs.g - rhs.g) * gWeight +
        square(lhs.b - rhs.b) * bWeight) /
        (rWeight + gWeight + bWeight) / MAX_SQUARE;
}
