#pragma once

#include <array>

namespace level
{

constexpr size_t height = 33;
constexpr size_t width = 28;

using info = std::array<std::array<char, width>, height>;

using wall_texture_offsets = std::array<std::array<int, width>, height>;
using wall_rotations = std::array<std::array<int, width>, height>;

}
