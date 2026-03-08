#pragma once

#include <cstdint>

struct RgbColor
{
    constexpr RgbColor() = default;
    constexpr RgbColor(uint8_t r, uint8_t g, uint8_t b) : r(r), g(g), b(b) {}

    uint8_t r = 0;
    uint8_t g = 0;
    uint8_t b = 0;
};
