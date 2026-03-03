#pragma once

#include <cassert>
#include <vector>

#include "pos.hpp"

template <typename T>
struct Mat
{
public:
    Mat() = default;
    Mat(int rows, int cols) : rows(rows), cols(cols), data(3 * rows * cols) {}

    bool contains(int row, int col) const
    {
        return row >= 0 && row < rows && col >= 0 && col < cols;
    }

    bool contains(const IPos& pos) const
    {
        return pos.y >= 0 && pos.y < rows && pos.x >= 0 && pos.x < cols;
    }

    template <typename U>
    const U& at(int row, int col) const
    {
        assert(contains(row, col));
        return *reinterpret_cast<const U*>(&(data.data()[row * col * 3]));
    }

    template <typename U>
    U& at(int row, int col)
    {
        assert(contains(row, col));
        return *reinterpret_cast<U*>(&(data.data()[row * col * 3]));
    }

    template <typename U>
    const U& at(const IPos& pos) const
    {
        assert(contains(pos));
        return *reinterpret_cast<const U*>(&(data.data()[pos.x * pos.y * 3]));
    }

    template <typename U>
    U& at(const IPos& pos)
    {
        assert(contains(pos));
        return *reinterpret_cast<U*>(&(data.data()[pos.x * pos.y * 3]));
    }

    int rows = 0;
    int cols = 0;
    std::vector<T> data;
};

