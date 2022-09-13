#include "systems/play_sound_effects.h"

#include "components/sound.h"
#include "components/sound_effect.h"
#include "registries.h"
#include "resources.h"

#include <SFML/Audio/Sound.hpp>
#include <spdlog/spdlog.h>

#include <memory>

namespace systems
{

using namespace entt::literals;

float constexpr listener_z = 300.f;
float constexpr attenuation = 8.f;
float constexpr min_distance_2d = 200.f;
float const min_distance_3d = std::sqrt(min_distance_2d * min_distance_2d + listener_z * listener_z);

void play_positioned_sound(
    sf::Sound& sound,
    sf::Vector3f const& listener_position)
{
    sound.setPosition(listener_position.x, -listener_position.y, 0.f);
    sound.setAttenuation(attenuation);
    sound.setMinDistance(min_distance_3d);

    sound.play();
}

void play_sound_effects()
{
    auto const sound_effects = registries::audio.view<const components::sound_effect>();
    sound_effects.each([](auto const e, auto const sound_effect)
    {
        auto const s = registries::audio.create();
        auto const r = resources::get(sound_effect.sound_effect);
        components::sound& sound = registries::audio.emplace<components::sound>(s, r, std::make_shared<sf::Sound>(r));

        auto const& listener_position = registries::audio.ctx().at<const sf::Vector3f>("listener_position"_hs);
        play_positioned_sound(*sound.sound, listener_position);
    });

    registries::audio.destroy(sound_effects.begin(), sound_effects.end());
}

}
