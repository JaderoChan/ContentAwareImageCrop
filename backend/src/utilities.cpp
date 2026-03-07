#include "utilities.hpp"

#include <cassert>
#include <cmath>
#include <array>
#include <algorithm>

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

std::vector<IPos> mapLineToOriginalSize(
    const std::vector<IPos>& scaledLine,
    int originalRows, int originalCols,
    int scaledRows, int scaledCols)
{
    std::vector<IPos> originalLine;
    originalLine.reserve(originalRows);

    double ratioX = static_cast<double>(originalCols) / scaledCols;
    double ratioY = static_cast<double>(originalRows) / scaledRows;

    for (int row = 0; row < originalRows; ++row)
    {
        // 找到原始图中当前行对应的缩放图中的行索引
        int scaledRow = std::clamp(static_cast<int>(std::round(row / ratioY)), 0, scaledRows - 1);

        // 获取缩放图中该行对应的列坐标，并映射回原图列坐标
        double scaledCol = scaledLine[scaledRow].x;
        int originalCol = std::clamp(static_cast<int>(std::round(scaledCol * ratioX)), 0, originalCols - 1);

        originalLine.emplace_back(originalCol, row);
    }

    return originalLine;
}

Image highlightLine(const Image& img, const std::vector<IPos>& line, const RgbColor& color)
{
    Image res = img;
    for (const auto& pt : line)
        res.at<RgbColor>(pt) = color;
    return res;
}

Image removeLine(const Image& img, const std::vector<IPos>& line)
{
    Image res(img.rows, img.cols - 1, img.channel);

    for (int row = 0; row < img.rows; ++row)
    {
        int colToRemove = line[row].x;

        const uint8_t* pSrcRow = &img.data()[row * img.cols * img.channel];
        uint8_t* pDstRow = &res.data()[row * res.cols * res.channel];

        int firstPartCount = colToRemove;
        if (firstPartCount > 0)
            memcpy(pDstRow, pSrcRow, firstPartCount * img.channel);

        int secondPartCount = img.cols - colToRemove - 1;
        if (secondPartCount > 0)
            memcpy(pDstRow + firstPartCount * img.channel,
                   pSrcRow + (colToRemove + 1) * img.channel,
                   secondPartCount * img.channel);
    }

    return res;
}

