#pragma once

#include "services/sound_player.h"
#include "services/astar_pathfinding.h"

#include <entt/locator/locator.hpp>

namespace services
{

using sound_player = entt::locator<sound::player>;
using path_finder = entt::locator<pathfinding::astar>;

void initialize();

}
