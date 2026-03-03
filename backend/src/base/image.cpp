#include "image.hpp"

#include "stb_image.h"

Image loadImageFromFile(const std::string& filepath)
{
    Image image;

    int channels;
    auto data = stbi_load(filepath.c_str(), &image.cols, &image.rows, &channels, 3);
    image.data.assign(data, data + image.cols * image.rows);
    stbi_image_free(data);

    return image;
}
