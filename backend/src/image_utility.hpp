#pragma ocne

#include <algorithm>

#include "base/mat.hpp"

/**
 * @brief 保持比例的缩小图像尺寸直到其尺寸符合给定的尺寸限制。
 * @note 如果所给图像是符合给定限制的则不做任何事。
 */
void limitImageScale(Image& img, int width, int height)
{
    Image scaled;

    double ratio = (double) width / img.width;
    ratio = (img.height * ratio > height ? (double) height / img.height : ratio);

    while (img.width > width)
    {

    }
}
