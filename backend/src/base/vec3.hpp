#pragma once

template <typename T>
struct Vec3
{
    Vec3() = default;
    Vec3(T a, T b, T c) : a(a), b(b), c(c) {}

    T a = T();
    T b = T();
    T c = T();
};
