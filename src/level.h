#pragma once

#include <array>

namespace level
{

constexpr size_t height = 33;
constexpr size_t width = 28;

template<typename T>
using grid = std::array<std::array<T, width>, height>;

using info = grid<char>;

using wall_texture_offsets = grid<int>;
using wall_rotations = grid<int>;

}
