#pragma once

#include "resources.h"

#include <SFML/Audio/Sound.hpp>
#include <SFML/System/NonCopyable.hpp>
#include <SFML/System/Vector2.hpp>

#include <list>

namespace sound
{

class player
    : private sf::NonCopyable
{
public:
    player();

    void play(
        resources::sound_effect const e);

    void play(
        resources::sound_effect const e,
        sf::Vector2f const position);

    void remove_stopped();

    void listener_position(
        sf::Vector2f const position);

    sf::Vector2f listener_position() const;

private:
    std::list<sf::Sound> sounds;
};

}
