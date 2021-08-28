#pragma once

#include "tomlpp.h"

#include <filesystem>

namespace configuration
{

using values_t = toml::table;

values_t const& values();

void initialize(
    std::filesystem::path const& config_file);

}
