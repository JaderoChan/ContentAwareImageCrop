#include "energy_mat.hpp"

#include "image_utility.hpp"

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
    return maxEnergy == 0.0 ? (energyMat * 0.0) : (energyMat * (1.0 / maxEnergy));
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
