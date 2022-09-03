#include "services/services.h"

#include "services/sound_player.h"

namespace services
{

void initialize()
{
    sound_player::emplace<sound::player>();
}

}
