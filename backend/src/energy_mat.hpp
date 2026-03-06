#pragma once

#include <base/image.hpp>

Mat<double> createEnergyMat(const Image& img);

void normalizeEnergyMat(Mat<double>& energyMat);

Image energyMatToGrayImage(const Mat<double>& energyMat);

std::vector<IPos> getMinimumEnergyLine(const Image& img);
