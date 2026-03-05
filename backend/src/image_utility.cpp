#include "image_utility.hpp"

#include <cassert>
#include <cmath>
#include <array>
#include <algorithm>

#include <base/color_similarity.hpp>
#include <base/pos.hpp>
#include <base/rgb.hpp>

Image limitImageScale(const Image& img, int width, int height)
{
    assert(width > 0 && height > 0 && !img.isEmpty());

    if (img.cols <= width && img.rows <= height)
        return img;

    double ratioW = static_cast<double>(width) / img.cols;
    double ratioH = static_cast<double>(height) / img.rows;
    double ratio = std::min(ratioW, ratioH);

    int newCols = std::max(1, static_cast<int>(std::floor(img.cols * ratio)));
    int newRows = std::max(1, static_cast<int>(std::floor(img.rows * ratio)));

    Image scaled(newRows, newCols, img.channel);

    for (int row = 0; row < newRows; ++row)
    {
        for (int col = 0; col < newCols; ++col)
        {
            double srcX = col / ratio;
            double srcY = row / ratio;

            int x1 = std::floor(srcX);
            int y1 = std::floor(srcY);
            int x2 = std::min(x1 + 1, img.cols - 1);
            int y2 = std::min(y1 + 1, img.rows - 1);

            double dx = srcX - x1;
            double dy = srcY - y1;

            const auto& p11 = img.at<RgbColor>(y1, x1);
            const auto& p12 = img.at<RgbColor>(y1, x2);
            const auto& p21 = img.at<RgbColor>(y2, x1);
            const auto& p22 = img.at<RgbColor>(y2, x2);

            auto& dst = scaled.at<RgbColor>(row, col);
            // 双线性插值
            double dyRem = 1.0 - dy;
            double dxRem = 1.0 - dx;
            dst.r = static_cast<uint8_t>(
                dyRem * (dxRem * p11.r + dx * p12.r) +
                dy * (dxRem * p21.r + dx * p22.r));
            dst.g = static_cast<uint8_t>(
                dyRem * (dxRem * p11.g + dx * p12.g) +
                dy * (dxRem * p21.g + dx * p22.g));
            dst.b = static_cast<uint8_t>(
                dyRem * (dxRem * p11.b + dx * p12.b) +
                dy * (dxRem * p21.b + dx * p22.b));
        }
    }

    return scaled;
}

double computePointEnergy(const Image& img, int row, int col,
    double rWeight, double gWeight, double bWeight)
{
    int validPoint = 0;
    double energy = 0.0;
    const auto& currPtColor = img.at<RgbColor>(row, col);

    std::array<IPos, 4> pts{
        IPos(col - 1, row), ///< Left
        IPos(col, row - 1), ///< Top
        IPos(col + 1, row), ///< Right
        IPos(col, row + 1), ///< Bottom
    };

    for (const auto& pt : pts)
    {
        if (img.contains(pt))
        {
            energy += computeColorSimilarity(
                currPtColor, img.at<RgbColor>(pt),
                rWeight, gWeight, bWeight
            );
            validPoint++;
        }
    }

    assert(validPoint != 0);
    return 1.0 - energy / validPoint;
}
