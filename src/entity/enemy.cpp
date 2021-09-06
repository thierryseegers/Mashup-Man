#include "entity/enemy.h"

#include "configuration.h"
#include "entity/entity.h"
#include "level.h"
#include "resources.h"
#include "scene.h"
#include "tomlpp.h"
#include "utility.h"

#include <magic_enum.hpp>
#include <SFML/Graphics.hpp>
#include <spdlog/spdlog.h>

#include <memory>
#include <string_view>
#include <vector>

namespace entity
{

void enemy::hit()
{
    mode_ = mode::dead;
}

void enemy::behave(
    mode const m)
{
    if(mode_ == m)
    {
        return;
    }

    mode_ = m;

    spdlog::info("{} changed to mode to {}.", name(), magic_enum::enum_name(mode_));

    if(mode_ == mode::scatter)
    {
        // Pick a random corner area as a target.
        static size_t const x[2] = {5ul, level::width - 5};
        static size_t const y[2] = {5ul, level::width - 5};

        float const c = x[utility::random(1)];
        float const r = y[utility::random(1)];

        target = {c * level::tile_size, r * level::tile_size};

        spdlog::info("{} targetting coordinates [{}, {}].", name(), target.x, target.y);
    }
}

void enemy::update_self(
    sf::Time const& dt,
    commands_t& commands)
{
    // if(life)
    // {
    //     // Update travel.
    //     if(travelled > current->distance)
    //     {
    //         ++current;
    //         travelled = 0;
    //     }

    //     float const rad = utility::to_radian(current->angle + 90.f);
    //     velocity = {speed * std::cos(rad), speed * std::sin(rad)};

    //     travelled += speed * dt.asSeconds();

    //     // Update attack.
    //     if(attack_countdown <= sf::Time::Zero)
    //     {
    //         commands.push(make_command<scene::projectiles>([=](scene::projectiles& layer, sf::Time const&)
    //         {
    //             attack(layer);
    //         }));

    //         play_local_sound(commands, resources::sound_effect::enemy_gunfire);

    //         attack_countdown += sf::seconds(1.f / attack_rate);
    //     }
    //     else
    //     {
    //         attack_countdown -= dt;
    //     }
    // }
    // else    // We dead.
    // {
    //     // Show an explosion.
    //     commands.push(make_command<scene::aircrafts>([=](scene::aircrafts& layer, sf::Time const&)
    //     {
    //         layer.attach<explosion>(world_position());
    //     }));

    //     // Sound an explosion.
    //     play_local_sound(commands, utility::random(1) ? resources::sound_effect::explosion_1 : resources::sound_effect::explosion_2);

    //     // Possibly drop loot.
    //     if(utility::random(2) == 0)
    //     {
    //         spdlog::info("Loot dropped.");

    //         commands.push(make_command<scene::projectiles>([=](scene::projectiles& layer, sf::Time const&)
    //         {
    //             std::unique_ptr<pickup::pickup> pickup;
    //             switch(utility::random(3))
    //             {
    //                 case 0:
    //                     pickup = std::make_unique<pickup::health>();
    //                     break;
    //                 case 1:
    //                     pickup = std::make_unique<pickup::missile_refill>();
    //                     break;
    //                 case 2:
    //                     pickup = std::make_unique<pickup::increase_spread>();
    //                     break;
    //                 case 3:
    //                     pickup = std::make_unique<pickup::increase_fire_rate>();
    //                     break;
    //             }

    //             pickup->setPosition(world_position());
    //             pickup->velocity = {0.f, 1.f};
    //             layer.attach(std::move(pickup));
    //         }));
    //     }
    // }

    character::update_self(dt, commands);
}

goomba::goomba()
    : enemy{
        sprite{
            resources::texture::enemies,
            std::vector<sf::IntRect>{{1, 28, 16, 16}, {18, 28, 16, 16}},
            sf::seconds(1.f),
            sprite::repeat::loop,
            configuration::values()["enemies"]["goomba"]["scale"].value_or<float>(1.f)},
        *configuration::values()["enemies"]["goomba"]["speed"].value<int>(),
        direction::left}
{}

direction goomba::fork(
    std::vector<sf::Vector2f> const& brother_positions,
    std::map<direction, sf::Vector2f> const& choices) const
{
    switch(mode_)
    {
        case  mode::scatter:
            return std::min_element(choices.begin(), choices.end(), [=](auto const& c1, auto const& c2)
                {
                    return utility::length(target - c1.second) < utility::length(target - c2.second);
                })->first;
            break;
        case mode::chase:
            {
                assert(brother_positions.size());

                auto const closest = std::min_element(brother_positions.begin(), brother_positions.end(), [=](auto const& p1, auto const& p2)
                {
                    return utility::length(getPosition() - p1) < utility::length(getPosition() - p2);
                });

                return std::min_element(choices.begin(), choices.end(), [=](auto const& c1, auto const& c2)
                {
                    return utility::length(*closest - c1.second) < utility::length(*closest - c2.second);
                })->first;
            }
            break;
        case mode::frightened:
            break;
        default:
            break;
    }

    return choices.begin()->first;
}

std::string_view goomba::name() const
{
    static std::string const name_{"goomba"};

    return name_;
}
// void avenger::attack(
//     scene::projectiles& layer) const
// {
//     add_projectile<bullet<hostile>>(layer, {0.f, 0.5f}, projectile::downward);
// }

// raptor::raptor()
//     : enemy{*configuration::values()["aircraft"]["raptor"]["starting_health"].value<int>(),
//             *configuration::values()["aircraft"]["raptor"]["speed"].value<int>(),
//             *configuration::values()["aircraft"]["raptor"]["attack_rate"].value<float>(),
//             to_flight_pattern(*configuration::values()["aircraft"]["raptor"]["flight_pattern"].as_array()),
//             *magic_enum::enum_cast<resources::texture>(*configuration::values()["aircraft"]["texture"].value<std::string_view>()),
//             utility::to_intrect(*configuration::values()["aircraft"]["raptor"]["texture_rect"].as_array())}
// {}

// void raptor::attack(
//     scene::projectiles& layer) const
// {
//     add_projectile<missile<hostile>>(layer, {0.f, 0.5f}, projectile::downward);
// }

}
