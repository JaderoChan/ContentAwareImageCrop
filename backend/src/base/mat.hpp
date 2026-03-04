#pragma once

#include <cassert>
#include <vector>

#include "pos.hpp"

template <typename T>
struct Mat
{
public:
    Mat() = default;
    Mat(int rows, int cols, int channel, const T& value = T())
        : rows(rows), cols(cols), channel(channel), _d(channel * rows * cols, value) {}
    Mat(int rows, int cols, int channel, const T* data, size_t dataSize)
        : rows(rows), cols(cols), channel(channel), _d(data, data + dataSize) {}

    typename std::vector<T>::const_iterator begin() const
    { return _d.begin(); }

    typename std::vector<T>::const_iterator end() const
    { return _d.end(); }

    typename std::vector<T>::iterator begin()
    { return _d.begin(); }

    typename std::vector<T>::iterator end()
    { return _d.end(); }

    const T* data() const
    { return _d.data(); }

    T* data()
    { return _d.data(); }

    template <typename U, typename Functor>
    void forEach(const Functor& operation)
    {
        auto* p = reinterpret_cast<unsigned char*>(data());
        auto* e = p + _d.size() * sizeof(T);

        assert((e - p) % sizeof(U) == 0);

        while (p < e)
        {
            operation(*reinterpret_cast<U*>(p));
            p += sizeof(U);
        }
    }

    bool isEmpty() const
    { return rows == 0 || cols == 0 || channel == 0 || _d.empty(); }

    bool contains(int row, int col) const
    { return row >= 0 && row < rows && col >= 0 && col < cols; }

    bool contains(const IPos& pos) const
    { return pos.y >= 0 && pos.y < rows && pos.x >= 0 && pos.x < cols; }

    template <typename U>
    const U& at(int row, int col) const
    {
        assert(contains(row, col));
        return *reinterpret_cast<const U*>(&(data()[(row * cols + col) * channel]));
    }

    template <typename U>
    U& at(int row, int col)
    {
        assert(contains(row, col));
        return *reinterpret_cast<U*>(&(data()[(row * cols + col) * channel]));
    }

    template <typename U>
    const U& at(const IPos& pos) const
    {
        assert(contains(pos));
        return *reinterpret_cast<const U*>(&(data()[(pos.y * cols + pos.x) * channel]));
    }

    template <typename U>
    U& at(const IPos& pos)
    {
        assert(contains(pos));
        return *reinterpret_cast<U*>(&(data()[(pos.y * cols + pos.x) * channel]));
    }

    Mat& operator+=(const T& v) { for (auto& i : _d) i += v; return *this; }
    Mat& operator-=(const T& v) { for (auto& i : _d) i -= v; return *this; }
    Mat& operator*=(const T& v) { for (auto& i : _d) i *= v; return *this; }
    Mat& operator/=(const T& v) { for (auto& i : _d) i /= v; return *this; }

    int rows = 0;
    int cols = 0;
    int channel = 0;

    std::vector<T> _d;
};

template <typename T>
Mat<T> operator+(const Mat<T>& mat, const T& v)
{
    Mat<T> result = mat;
    result += v;
    return result;
}

template <typename T>
Mat<T> operator-(const Mat<T>& mat, const T& v)
{
    Mat<T> result = mat;
    result -= v;
    return result;
}

template <typename T>
Mat<T> operator*(const Mat<T>& mat, const T& v)
{
    Mat<T> result = mat;
    result *= v;
    return result;
}

template <typename T>
Mat<T> operator/(const Mat<T>& mat, const T& v)
{
    Mat<T> result = mat;
    result /= v;
    return result;
}
