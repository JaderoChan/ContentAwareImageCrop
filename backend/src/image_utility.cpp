#include "image_utility.hpp"

#include <cassert>
#include <cmath>
#include <algorithm>

#include <base/pos.hpp>
#include <base/rgb.hpp>

/**
{pos.x - 1, pos.y - 1},
{pos.x, pos.y - 1},
{pos.x + 1, pos.y - 1},
{pos.x + 1, pos.y},
{pos.x + 1, pos.y + 1},
{pos.x, pos.y + 1},
{pos.x - 1, pos.y + 1},
{pos.x - 1, pos.y}
 */

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

    Image scaled(newRows, newCols);

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

            const auto& pl = img.at<RgbColor>(y1, x1);
            const auto& pt = img.at<RgbColor>(y1)

            auto& rgb = scaled.at<RgbColor>(row, col);
        }
    }

    return scaled;
}
