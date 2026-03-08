#include "image.hpp"

#include <stb_image.h>

Image loadImageFromFile(const std::string& filepath)
{
    int rows = 0, cols = 0, channel = 0;
    auto data = stbi_load(filepath.c_str(), &cols, &rows, &channel, 3);

    if (rows == 0 || cols == 0 || channel == 0)
    {
        stbi_image_free(data);
        return Image();
    }

    Image image(rows, cols, 3, data, rows * cols * 3);
    stbi_image_free(data);
    return image;
}
