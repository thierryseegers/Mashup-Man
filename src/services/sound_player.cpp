#include "services/sound_player.h"

#include "components/sound_effect.h"
#include "registries.h"
#include "resources.h"
#include "utility.h"

#include <SFML/Audio/Listener.hpp>
#include <SFML/Audio/Sound.hpp>
#include <SFML/System/Vector2.hpp>

#include <cmath>
#include <memory>

namespace services::sound
{

using namespace entt::literals;

float constexpr listener_z = 300.f;
float constexpr attenuation = 8.f;
float constexpr min_distance_2d = 200.f;
float const min_distance_3d = std::sqrt(min_distance_2d * min_distance_2d + listener_z * listener_z);

void play_positioned_sound(
    sf::Sound& sound,
    sf::Vector2f const& listener_position)
{
    sound.setPosition(listener_position.x, -listener_position.y, 0.f);
    sound.setAttenuation(attenuation);
    sound.setMinDistance(min_distance_3d);

    sound.play();
}

void player::play(
    resources::sound_effect const sound_effect)
{
    auto const& listener_position = registries::audio.ctx().at<const sf::Vector3f>("listener_position"_hs);
    play(sound_effect, sf::Vector2f{listener_position.x, listener_position.y});
}

void player::play(
    resources::sound_effect const sound_effect,
    sf::Vector2f const& position)
{
    auto const e = registries::audio.create();
    auto const r = resources::get(sound_effect);
    components::sound_effect& se = registries::audio.emplace<components::sound_effect>(e, r, std::make_shared<sf::Sound>(r));

    play_positioned_sound(*se.sound, position);
}

void player::pause()
{
    registries::audio.view<components::sound_effect>().each([](auto& sound_effect)
    {
        sound_effect.sound->pause();
    });
}

void player::resume()
{
    registries::audio.view<components::sound_effect>().each([](auto& sound_effect)
    {
        sound_effect.sound->play();
    });
}

}
