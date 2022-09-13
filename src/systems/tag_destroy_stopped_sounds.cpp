#include "systems/tag_destroy_stopped_sounds.h"

#include "components/destroy_tag.h"
#include "components/sound.h"
#include "registries.h"

#include <SFML/Audio/Sound.hpp>

namespace systems
{

void tag_destroy_stopped_sounds()
{
    registries::audio.view<const components::sound>().each([&](auto const e, auto const& sound)
    {
        if(sound.sound->getStatus() == sf::Sound::Stopped)
        {
            registries::audio.emplace<components::destroy_tag>(e);
        }
    });
}

}
