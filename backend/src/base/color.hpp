#pragma once

#include <algorithm>

#include "rgb.hpp"
#include "hsv.hpp"
#include "color_similarity.hpp"

inline RgbColor hsvToRgb(const HsvColor& hsv)
{
    int hi = hsv.h / 60.0;
    double f = hsv.h / 60.0 - hi;
    double p = hsv.v * (1.0 - hsv.s);
    double q = hsv.v * (1.0 - f * hsv.s);
    double t = hsv.v * (1.0 - (1.0 - f) * hsv.s);

    uint8_t nv = hsv.v * UINT8_MAX;
    uint8_t np = p * UINT8_MAX;
    uint8_t nq = q * UINT8_MAX;
    uint8_t nt = t * UINT8_MAX;

    switch (hi)
    {
        case 0:     return {nv, nt, nq};
        case 1:     return {nq, nv, np};
        case 2:     return {np, nv, nt};
        case 3:     return {np, nq, nv};
        case 4:     return {nt, np, nv};
        case 5:     return {nv, np, nq};
        default:    return RgbColor();
    }
}

inline HsvColor rgbToHsv(const RgbColor& rgb)
{
    HsvColor hsv;

    double nr = (double) rgb.r / UINT8_MAX;
    double ng = (double) rgb.g / UINT8_MAX;
    double nb = (double) rgb.b / UINT8_MAX;

    auto max = std::max({nr, ng, nb});
    auto min = std::min({nr, ng, nb});
    if (max == min)
        hsv.h = 0.0;
    else if (max == nr && ng >= nb)
        hsv.h = 60.0 * (ng - nb) / (max - min);
    else if (max == nr && ng < nb)
        hsv.h = 60.0 * (ng - nb) / (max - min) + 360.0;
    else if (max == ng)
        hsv.h = 60.0 * (nb - nr) / (max - min) + 120.0;
    else if (max == nb)
        hsv.h = 60.0 * (nr - ng) / (max - min) + 240.0;
    hsv.s = max == 0.0 ? 0.0 : (max - min) / max;
    hsv.v = max;

    return hsv;
}
