#include "sound.h"

#include "resources.h"
#include "utility.h"

#include <SFML/Audio/Listener.hpp>
#include <SFML/Audio/Sound.hpp>
#include <SFML/System/Vector2.hpp>

#include <cmath>

namespace sound
{

float constexpr listener_z = 300.f;
float constexpr attenuation = 8.f;
float constexpr min_distance_2d = 200.f;
float const min_distance_3d = std::sqrt(min_distance_2d * min_distance_2d + listener_z * listener_z);

player::player()
{
    auto& sound_buffers = utility::single::mutable_instance<resources::sound_buffers_t>();

    sound_buffers.load(resources::sound_effect::bump, "assets/sounds/Super Mario Bros 1 and 2(JP)/Bump.wav");
    sound_buffers.load(resources::sound_effect::collect_coin, "assets/sounds/Super Mario Bros 1 and 2(JP)/Coin.wav");
    sound_buffers.load(resources::sound_effect::collect_powerup, "assets/sounds/Super Mario Bros 1 and 2(JP)/Powerup.wav");
    sound_buffers.load(resources::sound_effect::die, "assets/sounds/Super Mario Bros 1 and 2(JP)/Die.wav");
    sound_buffers.load(resources::sound_effect::fireball, "assets/sounds/Super Mario Bros 1 and 2(JP)/Fire ball.wav");
    sound_buffers.load(resources::sound_effect::kick, "assets/sounds/Super Mario Bros 1 and 2(JP)/Kick.wav");
    sound_buffers.load(resources::sound_effect::pause, "assets/sounds/Super Mario Bros 1 and 2(JP)/Pause.wav");
    sound_buffers.load(resources::sound_effect::warp, "assets/sounds/Super Mario Bros 1 and 2(JP)/Warp.wav");

    auto const& die_buffer = sound_buffers.get(resources::sound_effect::die);
    auto const* die_samples = sound_buffers.get(resources::sound_effect::die).getSamples();
    sf::SoundBuffer short_die_buffer;
    short_die_buffer.loadFromSamples(die_samples, die_buffer.getSampleRate() * 0.6, die_buffer.getChannelCount(), die_buffer.getSampleRate());
    sound_buffers.copy(resources::sound_effect::short_die, short_die_buffer);
}

void player::play(
    resources::sound_effect const e)
{
    play(e, listener_position());
}

void player::play(
    resources::sound_effect const e,
    sf::Vector2f const position)
{
    sounds.push_back((sf::Sound{utility::single::instance<resources::sound_buffers_t>().get(e)}));
    auto& sound = sounds.back();

    sound.setPosition(position.x, -position.y, 0.f);
    sound.setAttenuation(attenuation);
    sound.setMinDistance(min_distance_3d);

    sound.play();
}

void player::remove_stopped()
{
    sounds.remove_if([](sf::Sound const& s)
    {
        return s.getStatus() == sf::Sound::Stopped;
    });
}

void player::listener_position(
    sf::Vector2f const position)
{
    sf::Listener::setPosition(position.x, -position.y, listener_z);
}

sf::Vector2f player::listener_position() const
{
    return {sf::Listener::getPosition().x, sf::Listener::getPosition().y};
}

}
