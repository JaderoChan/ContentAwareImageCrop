#include "content_aware_image_crop.hpp"

#include <energy_mat.hpp>
#include <utilities.hpp>

Image removeMinimumEnergyLines(const Image& img, int linesToRemove)
{
    Image res = img;

    for (int i = 0; i < linesToRemove; ++i)
    {
        Image scaledImg = limitImageScale(res, 512, 512);
        auto line = fetchMinimumEnergyLine(scaledImg);
        line = mapLineToOriginalSize(line, res.rows, res.cols, scaledImg.rows, scaledImg.cols);
        res = removeLine(res, line);
    }

    return res;
}
