#pragma once

#include <array>

namespace level
{

constexpr size_t height = 28;
constexpr size_t width = 28;

using info = std::array<std::array<int, width>, height>;

}
