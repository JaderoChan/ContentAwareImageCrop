#pragma once

#include <vector>

#include "pos.hpp"

template <typename T>
using Line2D = std::vector<Pos2D<T>>;

using ILine = Line2D<int>;
