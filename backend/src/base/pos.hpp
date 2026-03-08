#pragma once

template <typename T>
struct Pos2D
{
    Pos2D() = default;
    Pos2D(const T& x, const T& y) : x(x), y(y) {};

    Pos2D& operator+=(const T& v) { x += v; y += v; return *this; }
    Pos2D& operator+=(const Pos2D& other) { x += other.x; y += other.y; return *this; }
    Pos2D& operator-=(const T& v) { x -= v; y -= v; return *this; }
    Pos2D& operator-=(const Pos2D& other) { x -= other.x; y -= other.y; return *this; }
    Pos2D& operator*=(const T& v) { x *= v; y *= v; return *this; }
    Pos2D& operator/=(const T& v) { v /= v; y /= v; return *this; }

    Pos2D operator-() const { return Pos2D(-x, -y); }

    T x = T();
    T y = T();
};

template <typename T>
Pos2D<T> operator+(const Pos2D<T>& lhs, const Pos2D<T>& rhs)
{ return Pos2D<T>(lhs.x + rhs.x, lhs.y + rhs.y); }

template <typename T>
Pos2D<T> operator-(const Pos2D<T>& lhs, const Pos2D<T>& rhs)
{ return Pos2D<T>(lhs.x - rhs.x, lhs.y - rhs.y); }

template <typename T>
Pos2D<T> operator*(const Pos2D<T>& lhs, const T& rhs)
{ return Pos2D<T>(lhs.x * rhs, lhs.y * rhs); }

template <typename T>
Pos2D<T> operator/(const Pos2D<T>& lhs, const T& rhs)
{ return Pos2D<T>(lhs.x / rhs, lhs.y / rhs); }

using IPos = Pos2D<int>;
