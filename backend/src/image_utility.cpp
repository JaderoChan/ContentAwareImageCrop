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
    assert(width > 0 && height > 0);

    if (img.cols <= width && img.rows <= height)
        return img;

    double ratioW = static_cast<double>(width) / img.cols;
    double ratioH = static_cast<double>(height) / img.rows;
    double ratio = std::min(ratioW, ratioH);

    int newCols = std::max(1, static_cast<int>(std::floor(img.cols * ratio)));
    int newRows = std::max(1, static_cast<int>(std::floor(img.rows * ratio)));

    Image scaled(newRows, newCols, img.channel);

    double rowScale = static_cast<double>(img.rows) / newRows;
    double colScale = static_cast<double>(img.cols) / newCols;

    for (int dr = 0; dr < newRows; ++dr)
    {
        double srcRowStart = dr * rowScale;
        double srcRowEnd = (dr + 1) * rowScale;

        int rMin = static_cast<int>(std::floor(srcRowStart));
        int rMax = std::min(static_cast<int>(std::ceil(srcRowEnd)) - 1, img.rows - 1);

        for (int dc = 0; dc < newCols; ++dc)
        {
            double srcColStart = dc * colScale;
            double srcColEnd = (dc + 1) * colScale;

            int cMin = static_cast<int>(std::floor(srcColStart));
            int cMax = std::min(static_cast<int>(std::ceil(srcColEnd)) - 1, img.cols - 1);

            double sumR = 0.0, sumG = 0.0, sumB = 0.0;
            double totalWeight = 0.0;

            for (int sr = rMin; sr <= rMax; ++sr)
            {
                double rOverlap = std::min(static_cast<double>(sr + 1), srcRowEnd) -
                    std::max(static_cast<double>(sr), srcRowStart);

                for (int sc = cMin; sc <= cMax; ++sc)
                {
                    double cOverlap = std::min(static_cast<double>(sc + 1), srcColEnd) -
                        std::max(static_cast<double>(sc), srcColStart);

                    double weight = rOverlap * cOverlap;
                    const auto& rgb = img.at<RgbColor>(sr, sc);

                    sumR += rgb.r * weight;
                    sumG += rgb.g * weight;
                    sumB += rgb.b * weight;
                    totalWeight += weight;
                }
            }

            auto& dst = scaled.at<RgbColor>(dr, dc);
            dst.r = static_cast<uint8_t>(std::min(std::round(sumR / totalWeight), 255.0));
            dst.g = static_cast<uint8_t>(std::min(std::round(sumG / totalWeight), 255.0));
            dst.b = static_cast<uint8_t>(std::min(std::round(sumB / totalWeight), 255.0));
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
