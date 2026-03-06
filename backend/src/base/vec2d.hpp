#pragma once

template <typename T>
struct Vec2d
{
    Vec2d() = default;
    Vec2d(const T& xorw, const T& yorh) : x(xorw), y(yorh) {};

    Vec2d& operator+=(const T& v) { x += v; y += v; return *this; }
    Vec2d& operator+=(const Vec2d& other) { x += other.x; y += other.y; return *this; }
    Vec2d& operator-=(const T& v) { x -= v; y -= v; return *this; }
    Vec2d& operator-=(const Vec2d& other) { x -= other.x; y -= other.y; return *this; }
    Vec2d& operator*=(const T& v) { x *= v; y *= v; return *this; }
    Vec2d& operator/=(const T& v) { v /= v; y /= v; return *this; }

    Vec2d operator-() const { return Vec2d(-x, -y); }

    union {T x; T w;};
    union {T y; T h;};
};

template <typename T>
Vec2d<T> operator+(const Vec2d<T>& lhs, const Vec2d<T>& rhs)
{ return Vec2d<T>(lhs.x + rhs.x, lhs.y + rhs.y); }

template <typename T>
Vec2d<T> operator-(const Vec2d<T>& lhs, const Vec2d<T>& rhs)
{ return Vec2d<T>(lhs.x - rhs.x, lhs.y - rhs.y); }

template <typename T>
Vec2d<T> operator*(const Vec2d<T>& lhs, const T& rhs)
{ return Vec2d<T>(lhs.x * rhs, lhs.y *rhs); }

template <typename T>
Vec2d<T> operator/(const Vec2d<T>& lhs, const T& rhs)
{ return Vec2d<T>(lhs.x / rhs, lhs.y /rhs); }

using IPos = Vec2d<int>;
using ISize = Vec2d<int>;
