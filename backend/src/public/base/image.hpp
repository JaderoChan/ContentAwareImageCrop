#pragma once

#include <cstdint>
#include <string>

#include "mat.hpp"

using Image = Mat<uint8_t>;

Image loadImageFromFile(const std::string& filepath);
