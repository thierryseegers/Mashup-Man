#pragma once

#include "services/sound_player.h"

#include <entt/locator/locator.hpp>

namespace services
{

using sound_player = entt::locator<sound::player>;

void initialize();

}
