#pragma ocne

#include <vector>

#include "base/image.hpp"

/**
 * @brief 保持比例的缩小图像尺寸直到其尺寸符合给定的尺寸限制。
 * @note 如果所给图像是符合给定限制的则不做任何事。
 */
void limitImageScale(Image& img, int width, int height);
