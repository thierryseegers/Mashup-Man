#include "systems/tag_destroy_stopped_sounds.h"

#include "components/destroy_tag.h"
#include "components/sound_effect.h"
#include "registries.h"

#include <SFML/Audio/Sound.hpp>

namespace systems
{

void tag_destroy_stopped_sounds()
{
    registries::audio.view<const components::sound_effect>().each([&](auto const e, auto const& sound_effect)
    {
        if(sound_effect.sound->getStatus() == sf::Sound::Stopped)
        {
            registries::audio.emplace<components::destroy_tag>(e);
        }
    });
}

}
