#include "energy_mat.hpp"

#include <cfloat>
#include <array>

#include "color_similarity.hpp"
#include "utilities.hpp"

double computePointEnergy(
    const Image& img, int row, int col,
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

    return validPoint == 0 ? 0.0 : 1.0 - energy / validPoint;
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

std::vector<IPos> fetchMinimumEnergyLine(const Image& img, int colLow, int colHigh)
{
    if (colHigh < 0)
        colHigh = img.cols;

    // 参数无效时返回空
    if (img.isEmpty() || img.channel != 3 || colLow < 0 || colHigh > img.cols || colHigh <= colLow)
        return {};

    // DP 矩阵覆盖 [colLow, colHigh) 范围，列索引为局部索引 lc = col - colLow。
    const int rangeWidth = colHigh - colLow;
    Mat<double> energyDp(img.rows, rangeWidth, 1, DBL_MAX);
    Mat<int8_t> offsetDp(img.rows, rangeWidth, 1, 0);

    double minEnergy = DBL_MAX;
    int minEnergyLastLc = 0;

    // 初始化第一行
    for (int lc = 0; lc < rangeWidth; ++lc)
        energyDp.at<double>(0, lc) = computePointEnergy(img, 0, colLow + lc);

    for (int row = 1; row < img.rows; ++row)
    {
        for (int lc = 0; lc < rangeWidth; ++lc)
        {
            double& energy = energyDp.at<double>(row, lc);
            int8_t& direct = offsetDp.at<int8_t>(row, lc);

            for (int d = -1; d <= 1; ++d)
            {
                int nlc = lc + d;
                if (nlc < 0 || nlc >= rangeWidth)
                    continue;

                double prev = energyDp.at<double>(row - 1, nlc);
                if (prev < energy)
                {
                    energy = prev;
                    direct = static_cast<int8_t>(d);
                }
            }

            energy += computePointEnergy(img, row, colLow + lc);

            if (row == img.rows - 1 && energy < minEnergy)
            {
                minEnergy = energy;
                minEnergyLastLc = lc;
            }
        }
    }

    std::vector<IPos> line(img.rows);
    int lc = minEnergyLastLc;
    for (int row = img.rows - 1; row >= 0; --row)
    {
        line[row] = IPos(colLow + lc, row);
        lc = std::clamp(lc + static_cast<int>(offsetDp.at<int8_t>(row, lc)), 0, rangeWidth - 1);
    }

    return line;
}

