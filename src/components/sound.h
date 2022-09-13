#pragma once

#include <resources.h>
#include <SFML/Audio/Sound.hpp>

#include <memory>

namespace components
{

struct sound
{
    entt::resource<sf::SoundBuffer> buffer;
    // When components get compacted, `operator=` is invoked and `operator=(sf::Sound)`
    // stops playback hence the use of `shared_ptr<sf::Sound>`.
    std::shared_ptr<sf::Sound> sound;
};

}
