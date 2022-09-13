#include "services/sound_player.h"

#include "components/sound.h"
#include "components/sound_effect.h"
#include "registries.h"
#include "resources.h"
#include "utility.h"

#include <SFML/Audio/Listener.hpp>
#include <SFML/Audio/Sound.hpp>
#include <SFML/System/Vector2.hpp>

#include <cmath>

namespace services::sound
{

void player::play(
    resources::sound_effect const sound_effect)
{
    auto const& listener_position = registries::audio.ctx().at<const sf::Vector3f>("listener_position"_hs);
    play(sound_effect, sf::Vector2f{listener_position.x, listener_position.y});
    entt::entity const e = registries::audio.create();
    registries::audio.emplace<components::sound_effect>(e, sound_effect);
}

void player::play(
    resources::sound_effect const sound_effect,
    sf::Vector2f const& position)
{
    entt::entity const e = registries::audio.create();
    registries::audio.emplace<components::positioned_sound_effect>(e, sound_effect, sf::Vector3f{position.x, position.y, 0.f});
}

void player::pause()
{
    registries::audio.view<components::sound>().each([](auto& sound)
    {
        sound.sound->pause();
    });
}

void player::resume()
{
    registries::audio.view<components::sound>().each([](auto& sound)
    {
        sound.sound->play();
    });
}

}
