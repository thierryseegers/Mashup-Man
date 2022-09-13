#pragma once

#include "resources.h"

#include <SFML/System/NonCopyable.hpp>
#include <SFML/System/Vector2.hpp>

namespace services::sound
{

class player
    : private sf::NonCopyable
{
public:
    void play(
        resources::sound_effect const);

    void play(
        resources::sound_effect const,
        sf::Vector2f const&);

    void pause();

    void resume();
};

}
