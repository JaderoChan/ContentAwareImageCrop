#pragma once

#include <cstdint>

#include "rgb.hpp"

struct HsvColor
{
    constexpr HsvColor() = default;
    constexpr HsvColor(double h, double s, double v) : h(h), s(s), v(v) {}

    double h = 0.0; ///< Range in [0.0, 360.0).
    double s = 0.0; ///< Range in [0.0, 1.0].
    double v = 0.0; ///< Range in [0.0, 1.0].
};
