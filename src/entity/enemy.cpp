#include "entity/enemy.h"

#include "configuration.h"
// #include "entity/bullet.h"
#include "entity/entity.h"
// #include "entity/explosion.h"
// #include "entity/flight.h"
// #include "entity/missile.h"
#include "entity/pickup.h"
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

// flight::pattern to_flight_pattern(
//     toml::array const& values)
// {
//     flight::pattern pattern;

//     for(auto const& node : values)
//     {
//         auto const& move = *node.as_array();
//         pattern.push_back({*move[0].value<float>(), *move[1].value<float>()});
//     }

//     return pattern;
// }

enemy::enemy(
    // int const starting_life,
    // int const speed,
    // float const attack_rate,
    // flight::pattern const& pattern,
    resources::texture const& texture,
    sf::IntRect const& texture_rect,
    float const& scale)
    : hostile<character>{texture, texture_rect, scale}
    // , speed{speed}
    // , pattern{pattern}
    // , current{this->pattern, this->pattern.begin()}
    // , travelled{0}
    // , attack_rate{attack_rate}
{}

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
    : enemy{*magic_enum::enum_cast<resources::texture>(*configuration::values()["enemies"]["texture"].value<std::string_view>()),
            utility::to_intrect(*configuration::values()["enemies"]["goomba"]["texture_rect"].as_array()),
             configuration::values()["enemies"]["goomba"]["scale"].value_or<float>(1.f)}
{}

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
