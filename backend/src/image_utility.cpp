#include "image_utility.hpp"

#include "base/pos.hpp"

/**
{pos.x - 1, pos.y - 1},
{pos.x, pos.y - 1},
{pos.x + 1, pos.y - 1},
{pos.x + 1, pos.y},
{pos.x + 1, pos.y + 1},
{pos.x, pos.y + 1},
{pos.x - 1, pos.y + 1},
{pos.x - 1, pos.y}
 */

void limitImageScale(Image& img, int width, int height)
{
    double ratio = (double) width / img.cols;
    ratio = (img.rows * ratio > height ? (double) height / img.rows : ratio);

    int newWidth = img.cols * ratio;
    int newHeight = img.rows * ratio;

    Image scaled(newWidth, newHeight);
}
