#include "energy_mat.hpp"

#include <cfloat>
#include <array>

#include "color_similarity.hpp"
#include "utilities.hpp"

double computePointEnergy(const Image& img, int row, int col,
    double rWeight, double gWeight, double bWeight)
{
    int validPoint = 0;
    double energy = 0.0;
    const auto& currPtColor = img.at<RgbColor>(row, col);

    std::array<IPos, 4> pts{
        IPos(col - 1, row), // Left
        IPos(col, row - 1), // Top
        IPos(col + 1, row), // Right
        IPos(col, row + 1), // Bottom
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

Mat<double> createEnergyMat(const Image& img)
{
    Mat<double> energyMat(img.rows, img.cols, 1);

    for (int row = 0; row < img.rows; ++row)
    {
        for (int col = 0; col < img.cols; ++col)
        {
            energyMat.at<double>(row, col) = computePointEnergy(img, row, col);
        }
    }

    return energyMat;
}

Mat<double> normalizeEnergyMat(const Mat<double>& energyMat)
{
    double maxEnergy = 0.0;
    for (const auto& v : energyMat)
        maxEnergy = std::max(maxEnergy, v);

    if (maxEnergy == 0.0)
        return energyMat;
    return energyMat * (1.0 / maxEnergy);
}

Image energyMatToGrayImage(const Mat<double>& energyMat)
{
    Image img(energyMat.rows, energyMat.cols, 1);

    for (int row = 0; row < energyMat.rows; ++row)
    {
        for (int col = 0; col < energyMat.cols; ++col)
        {
            img.at<uint8_t>(row, col) = static_cast<uint8_t>(UINT8_MAX * energyMat.at<double>(row, col));
        }
    }

    return img;
}

std::vector<IPos> fetchMinimumEnergyLine(const Image& img)
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

