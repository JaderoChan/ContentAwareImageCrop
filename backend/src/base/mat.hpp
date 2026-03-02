#pragma once

#include <cassert>
#include <string>
#include <vector>

#include "color.hpp"
#include "line.hpp"

template <typename T>
struct Mat
{
public:
    Mat() = default;
    Mat(int width, int height) : width(width), height(height), data(3 * width * height) {}

    template <typename U>
    const U& at(int row, int col) const
    {
        assert(row >= 0 && row < height);
        assert(col >= 0 && col < width);

        return *reinterpret_cast<const U*>(&(data.data()[row * col * 3]));
    }

    template <typename U>
    U& at(int row, int col)
    {
        assert(row >= 0 && row < height);
        assert(col >= 0 && col < width);

        return *reinterpret_cast<U*>(&(data.data()[row * col * 3]));
    }

    int width = 0;
    int height = 0;
    std::vector<T> data;
};

using Image = Mat<uint8_t>;

Image loadImageFromFile(const std::string& filepath);
