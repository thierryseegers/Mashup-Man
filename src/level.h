#pragma once

#include <array>

namespace level
{

constexpr size_t height = 31;
constexpr size_t width = 28;

constexpr int tile_size = 20;
constexpr int half_tile_size = tile_size / 2;

template<typename T>
using grid = std::array<std::array<T, width>, height>;

using description = grid<char>;

}
