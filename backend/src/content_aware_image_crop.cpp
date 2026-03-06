#include "content_aware_image_crop.hpp"

#include "energy_mat.hpp"
#include "image_utility.hpp"

Image removeMinimumEnergyLines(const Image& img, int lines)
{
    Image res = img;
    for (int i = 0; i < lines; ++i)
    {
        Image scaledImg = limitImageScale(res, 512, 512);
        auto line = getMinimumEnergyLine(scaledImg);
        line = mapLineToOriginalSize(line, ISize(res.cols, res.rows), ISize(scaledImg.cols, scaledImg.rows));
        res = removeLine(res, line);
    }
    return res;
}
