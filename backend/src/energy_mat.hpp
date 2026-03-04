#pragma once

#include <base/image.hpp>

Mat<double> createEnergyMat(const Image& img);

Mat<double> normalizeEnergyMat(const Mat<double>& energyMat);

Image energyMatToGrayImage(const Mat<double>& energyMat);
