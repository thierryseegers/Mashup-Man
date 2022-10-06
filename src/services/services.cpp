#include "services/services.h"

#include "services/sound_player.h"
#include "services/astar_pathfinding.h"

namespace services
{

void initialize()
{
    sound_player::emplace<sound::player>();
    path_finder::emplace<pathfinding::astar>();
}

}
