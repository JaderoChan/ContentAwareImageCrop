#pragma once

#include <cstdint>
#include <vector>

#include <base/image.hpp>
#include <base/pos.hpp>
#include <base/rgb.hpp>

std::vector<IPos> getMinimumEnergyLine(const Image& img);

Image highlightLine(const Image& img, const std::vector<IPos>& line, const RgbColor& color);
