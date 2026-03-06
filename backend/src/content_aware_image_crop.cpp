#include "content_aware_image_crop.hpp"

#include <cfloat>
#include <array>

#include "image_utility.hpp"

std::vector<IPos> getMinimumEnergyLine(const Image& img)
{
    assert(img.rows > 2 && img.cols > 2 && img.channel == 3 && !img.isEmpty());

    Mat<double> energyDp(img.rows, img.cols, 1, DBL_MAX);
    Mat<int8_t> offsetDp(img.rows, img.cols, 1, 0);

    double minEnergy = DBL_MAX;
    int minEnergyLastCol = 0;

    // 初始化 energyDp 第一行的能量值
    for (int col = 0; col < img.cols; ++col)
        energyDp.at<double>(0, col) = computePointEnergy(img, 0, col);

    for (int row = 1; row < img.rows; ++row)
    {
        for (int col = 0; col < img.cols; ++col)
        {
            double& energy = energyDp.at<double>(row, col);
            int8_t& direct = offsetDp.at<int8_t>(row, col);

            std::array<IPos, 3> pts{
                IPos(col - 1, row - 1), // Left top
                IPos(col, row - 1),     // Top
                IPos(col + 1, row - 1)  // Right top
            };

            for (int i = 0; i < 3; ++i)
            {
                if (energyDp.contains(pts[i]) && energyDp.at<double>(pts[i]) < energy)
                {
                    energy = energyDp.at<double>(pts[i]);
                    direct = i - 1;
                }
            }

            energy += computePointEnergy(img, row, col);

            if (row == img.rows - 1)
            {
                if (energy < minEnergy)
                {
                    minEnergy = energy;
                    minEnergyLastCol = col;
                }
            }
        }
    }

    std::vector<IPos> line(energyDp.rows);
    int col = minEnergyLastCol;
    for (int row = energyDp.rows - 1; row >= 0; --row)
    {
        line[row] = IPos(col, row);
        col = col + offsetDp.at<int8_t>(row, col);
    }

    return line;
}

Image highlightLine(const Image& img, const std::vector<IPos>& line, const RgbColor& color)
{
    Image result = img;
    for (const auto& pt : line)
        result.at<RgbColor>(pt) = color;
    return result;
}
