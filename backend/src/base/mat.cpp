#include "mat.hpp"

#include "stb_image.h"

Image loadImageFromFile(const std::string& filepath)
{
    Image image;

    int channels;
    auto data = stbi_load(filepath.c_str(), &image.width, &image.height, &channels, 3);
    image.data.assign(data, data + image.width * image.height);
    stbi_image_free(data);

    return image;
}
