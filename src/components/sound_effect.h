#pragma once

#include "resources.h"

#include <SFML/System/Vector3.hpp>

namespace components
{

struct sound_effect
{
    resources::sound_effect sound_effect;
};

struct positioned_sound_effect :
    public sound_effect
{
    sf::Vector3f listener_position;
};

}
