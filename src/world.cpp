#include "world.h"

#include "command.h"
#include "configuration.h"
// #include "effects/bloom.h"
// #include "effects/post_effect.h"
#include "entity/character.h"
#include "entity/entities.h"
// #include "particle.h"
#include "maze.h"
#include "resources.h"
#include "scene.h"
#include "utility.h"

#include <magic_enum.hpp>
#include <SFML/Graphics.hpp>
#include <spdlog/spdlog.h>

#include <algorithm>
#include <array>
#include <cmath>
#include <memory>
#include <numeric>
#include <tuple>
#include <vector>

world_t::world_t(
    sf::RenderTarget& output_target,
    sound::player& sound)
    : target{output_target}
    , view{output_target.getDefaultView()}
    , sound{sound}
    , mario{nullptr}
    , luigi{nullptr}
    , immovables{{}}
{
    build_scene();

    // view.setCenter(player_spawn_point);
}

commands_t& world_t::commands()
{
    return commands_;
}

// bool world_t::player_alive() const
// {
//     return !player->remove;
// }

// bool world_t::player_reached_end() const
// {
//     return !bounds.contains(player->getPosition());
// }


std::tuple<level::info, level::wall_texture_offsets, level::wall_rotations> read_level(
    std::filesystem::path const& path)
{
    level::info level_info;
    level::wall_texture_offsets wall_texture_offsets;
    level::wall_rotations wall_rotations;

    // Read in the maze tile information.
    std::ifstream file{path.c_str()};
    std::string line;
    for(auto& row : level_info)
    {
        std::getline(file, line);
        std::copy(line.begin(), line.end(), row.begin());
    }

    // Read in wall tile rotation information.
    for(auto& row : wall_rotations)
    {
        size_t column = 0;
        std::getline(file, line);
        std::for_each(line.begin(), line.end(), [&](char const c)
        {
            switch(c)
            {
                case '0':
                case '1':
                case '2':
                case '3':
                    row[column] = c - '0';
                    break;
                default:
                    row[column] = 0;
                    break;
            }
            ++column;
        });
        column = 0;
    }

    // Extract just the wall tile information.
    for(size_t r = 0; r != level::height; ++r)
    {
        for(size_t c = 0; c != level::width; ++c)
        {
            switch(level_info[r][c])
            {
                case '0':
                case '1':
                case '2':
                case '3':
                    wall_texture_offsets[r][c] = level_info[r][c] - '0';
                    break;
                default:
                    wall_texture_offsets[r][c] = 4;
                    break;
            }
        }
    }

    return {level_info, wall_texture_offsets, wall_rotations};
}

void world_t::build_scene()
{
    // Read information of the first level.
    level::wall_texture_offsets wall_texture_offsets;
    level::wall_rotations wall_rotations;
    std::tie(level_info, wall_texture_offsets, wall_rotations) = read_level("assets/levels/1.txt");

    // Create a sound player.
    graph.attach<scene::sound_t>(sound);

    // Create layers.
    layers[magic_enum::enum_integer(layer::id::maze)] = graph.attach<layer::maze>();
    layers[magic_enum::enum_integer(layer::id::items)] = graph.attach<layer::items>();
    layers[magic_enum::enum_integer(layer::id::characters)] = graph.attach<layer::characters>();
    layers[magic_enum::enum_integer(layer::id::projectiles)] = graph.attach<layer::projectiles>();
    layers[magic_enum::enum_integer(layer::id::pipes)] = graph.attach<layer::pipes>();

    auto *m = layers[magic_enum::enum_integer(layer::id::maze)]->attach<maze>();
    m->layout(wall_texture_offsets, wall_rotations);

    // Create entities as the level dictates.
    for(size_t r = 0; r != level::height; ++r)
    {
        for(size_t c = 0; c != level::width; ++c)
        {
            entity::entity *e = nullptr;
            switch(level_info[r][c])
            {
                case '.':
                    e = layers[magic_enum::enum_integer(layer::id::items)]->attach<entity::pickup::coin>();
                    immovables[r][c] = e;
                    break;
                case 'f':
                    e = layers[magic_enum::enum_integer(layer::id::items)]->attach<entity::pickup::flower>();
                    immovables[r][c] = e;
                    break;
                case 'g':
                    e = layers[magic_enum::enum_integer(layer::id::characters)]->attach<entity::goomba>();
                    break;
                case 'm':
                    e = layers[magic_enum::enum_integer(layer::id::items)]->attach<entity::pickup::mushroom>();
                    immovables[r][c] = e;
                    break;
                case 'p':
                    e = layers[magic_enum::enum_integer(layer::id::pipes)]->attach<entity::pipe>();
                    immovables[r][c] = e;

                    if(c == 0)
                    {
                        e->setRotation(180);
                    }
                    break;
                case 'x':
                    e = mario = layers[magic_enum::enum_integer(layer::id::characters)]->attach<entity::mario>();
                    characters.push_back(mario);
                    break;
                case 'y':
                    e = luigi = layers[magic_enum::enum_integer(layer::id::characters)]->attach<entity::luigi>();
                    characters.push_back(luigi);
                    break;
            }

            if(e)
            {
                e->setPosition(c * 20.f + 10.f, r * 20.f + 10.f);
            }
        }
    }

    // // Create the particle systems.
    // layers[layer::projectiles]->attach<particles<smoke>>(resources::textures().get(resources::texture::particle));
    // layers[layer::projectiles]->attach<particles<propellant>>(resources::textures().get(resources::texture::particle));
}

template<typename Entity1, typename Entity2>
std::pair<Entity1*, Entity2*> match(std::pair<scene::node*, scene::node*> const& p)
{
    if(auto *pa = dynamic_cast<Entity1*>(p.first))
    {
        if(auto *pb = dynamic_cast<Entity2*>(p.second))
        {
            return {pa, pb};
        }
    }
    else if(auto *pa = dynamic_cast<Entity2*>(p.first))
    {
        if(auto *pb = dynamic_cast<Entity1*>(p.second))
        {
            return {pb, pa};
        }
    }

    return {nullptr, nullptr};
}

void world_t::handle_collisions()
{
    std::set<std::pair<scene::node*, scene::node*>> collisions;

    // Detect collisions between characters (ought to be only the brothers) and pickups and pipes.
    for(auto* const bro : std::initializer_list<entity::brother*>{mario, luigi})
    {
        if(bro)
        {
            size_t const r = bro->getPosition().y / 20, c = bro->getPosition().x / 20;
            if(auto* const immovable = immovables[r][c]; immovable && bro->collides(immovable))
            {
                collisions.insert(std::minmax<scene::node*>(bro, immovable));
            }
        }
    }

    // Detect collisions between characters and projectiles.
    for(auto* const character : layers[magic_enum::enum_integer(layer::id::characters)]->children())
    {
        for(auto* const projectile : layers[magic_enum::enum_integer(layer::id::projectiles)]->children())
        {
            if(character->collides(projectile))
            {
                collisions.insert(std::minmax<scene::node*>(character, projectile));
            }
        }
    }

    for(auto const& collision : collisions)
    {
        // if(auto [aircraft, projectile] = match<entity::hostile<entity::character>, entity::friendly<entity::projectile>>(collision); aircraft && projectile)
        // {
        //     spdlog::info("Friendly shot a hostile!");
        //     aircraft->damage(projectile->damage);
        //     projectile->remove = true;
        // }
        // else if(auto [aircraft, projectile] = match<entity::friendly<entity::character>, entity::hostile<entity::projectile>>(collision); aircraft && projectile)
        // {
        //     spdlog::info("Friendly got shot!");
        //     aircraft->damage(projectile->damage);
        //     projectile->remove = true;
        // }
        // else 
        if(auto [bro, projectile] = match<entity::brother, entity::projectile>(collision); bro && projectile)
        {
            if(!projectile->remove)
            {
                projectile->remove = true;
                projectiles.remove(projectile);

                bro->hit();
            }
        }
        if(auto [e, projectile] = match<entity::enemy, entity::projectile>(collision); e && projectile)
        {
            if(!projectile->remove)
            {
                projectile->remove = true;
                projectiles.remove(projectile);

                e->remove = true;
            }
        }
        else if(auto [bro, pickup] = match<entity::brother, entity::pickup::pickup>(collision); bro && pickup)
        {
            if(!pickup->remove)
            {
                pickup->remove = true;
                immovables[pickup->getPosition().y / 20][pickup->getPosition().x / 20] = nullptr;

                pickup->apply(*bro);
                sound.play(pickup->sound_effect());
            }
        }
        else if(auto [bro, pipe_] = match<entity::brother, entity::pipe>(collision); bro && pipe_)
        {
            sound.play(resources::sound_effect::warp);
            // TODO: change hard-code to soft-code.
            if(pipe_->getPosition().x == 10)
            {
                bro->setPosition(27 * 20 - 10, bro->getPosition().y);
            }
            else
            {
                bro->setPosition(1 * 20 + 10, bro->getPosition().y);
            }
        }
    }
}

// void world_t::guide_missiles()
// {
//     // Setup command that stores all enemies in container.
//     commands_.push(make_command<entity::enemy>([=](entity::enemy& e, sf::Time const&)
//         {
//             enemies.push_back(&e);
//         }));

//     commands_.push(make_command<entity::guided_missile<entity::friendly>>([=](entity::guided_missile<entity::friendly>& gm, sf::Time const&)
//         {
//             if(!enemies.empty())
//             {
//                 auto e =std::min_element(enemies.begin(), enemies.end(), [&](entity::enemy* e1, entity::enemy* e2)
//                     {
//                         return scene::distance(gm, *e1) < scene::distance(gm, *e2);
//                     });

//                 gm.guide((*e)->world_position());
//             }
//         }));

//     enemies.clear();
// }

// sf::FloatRect world_t::view_bounds() const
// {
//     return sf::FloatRect(view.getCenter() - view.getSize() / 2.f, view.getSize());
// }

// sf::FloatRect world_t::battlefield_bounds() const
// {
//     // Return view bounds + some area at top, where enemies spawn.
//     sf::FloatRect bounds = view_bounds();
//     bounds.top -= 100.f;
//     bounds.height += 100.f;

//     return bounds;
// }

void world_t::update(
    sf::Time const dt)
{
    // // Guide guided missiles.
    // guide_missiles();

    // Dispatch commands.
    while(!commands_.empty())
    {
        graph.on_command(commands_.front(), dt);
        commands_.pop();
    }

    // Control Mario's direction given his desired direction (steering), current direction (heading) and any walls.
    auto const position = mario->getPosition();
    auto const heading = mario->heading();
    auto const steering = mario->steering();
    if((int)position.x % 20 >= 8 && (int)position.x % 20 <= 12 && (int)position.y % 20 >= 8 && (int)position.y % 20 <= 12)
    {
        // If Mario wants to change direction and he can, let him.
        if((steering != heading || mario->speed() == 0.f) &&
           ((steering == direction::right  && !utility::any_of(level_info[position.y / 20][position.x / 20 + 1], '0', '1', '2', '3')) ||
            (steering == direction::left   && !utility::any_of(level_info[position.y / 20][position.x / 20 - 1], '0', '1', '2', '3')) ||
            (steering == direction::down   && !utility::any_of(level_info[position.y / 20 + 1][position.x / 20], '0', '1', '2', '3')) ||
            (steering == direction::up     && !utility::any_of(level_info[position.y / 20 - 1][position.x / 20], '0', '1', '2', '3'))))
        {
            spdlog::info("Changing heading at coordinates [{}, {}] to [{}]", position.x, position.y, magic_enum::enum_name(steering));
            mario->head(steering);
            mario->throttle(1.f);
            mario->setPosition(((int)position.x / 20) * 20 + 10, ((int)position.y / 20) * 20 + 10);
        }
        // If Mario is crusing along and he's about to face a wall, stop him.
        else if(mario->speed() != 0.f &&
                ((heading == direction::right    && utility::any_of(level_info[position.y / 20][position.x / 20 + 1], '0', '1', '2', '3')) ||
                 (heading == direction::left     && utility::any_of(level_info[position.y / 20][position.x / 20 - 1], '0', '1', '2', '3')) ||
                 (heading == direction::down     && utility::any_of(level_info[position.y / 20 + 1][position.x / 20], '0', '1', '2', '3')) ||
                 (heading == direction::up       && utility::any_of(level_info[position.y / 20 - 1][position.x / 20], '0', '1', '2', '3'))))
        {
            spdlog::info("Hit a wall at coordinates [{}, {}] heading [{}]", position.x, position.y, magic_enum::enum_name(heading));
            mario->throttle(0.f);
        }
        // Else we let him cruise along.
    }

    // If a fireball hits a wall or a pipe, remove it.
    for(auto* const projectile : layers[magic_enum::enum_integer(layer::id::projectiles)]->children())
    {
        auto const r = projectile->getPosition().y / 20, c = projectile->getPosition().x/ 20;
        if(utility::any_of(level_info[r][c], '0', '1', '2', '3', 'p'))
        {
            if(auto* fireball = dynamic_cast<entity::fireball*>(projectile))
            {
                fireball->remove = true;
                sf::Vector2f offset;
                switch(fireball->heading())
                {
                    case direction::right:
                        offset = {7.f, 0.f};
                        break;
                    case direction::left:
                        offset = {-7.f, 0.f};
                        break;
                    case direction::down:
                        offset = {0.f, 7.f};
                        break;
                    case direction::up:
                        offset = {0.f, -7.f};
                        break;
                    default:
                        break;
                }
                layers[magic_enum::enum_integer(layer::id::projectiles)]->attach<entity::fizzle>()->setPosition(fireball->getPosition() + offset);
                sound.play(resources::sound_effect::bump);
            }
        }
    }

    // // Prevent the player from going off-screen.
    // sf::FloatRect const bounds{view.getCenter() - view.getSize() / 2.f, view.getSize()};
    // float const border_distance = 40.f;

    // auto position = player->getPosition();
    // position.x = std::clamp(position.x, bounds.left + border_distance, bounds.left + bounds.width - border_distance);
    // position.y = std::clamp(position.y, bounds.top + border_distance, bounds.top + bounds.height - border_distance);

    // player->setPosition(position);

    // Deal with collision.
    handle_collisions();

    // Remove all destroyed entities, spawn new enemies if need be.
    graph.sweep_removed();
    // spawn_enemies();

    // Remove played sounds and reposition player in sound space.
    sound.remove_stopped();
    // sound.listener_position(player->world_position());

    // Update the entire graph.
    graph.update(dt, commands_);
}

void world_t::draw()
{
    // if(effect::post_effect::supported())
    // {
    //     scene_texture.clear();
    //     scene_texture.setView(view);
    //     scene_texture.draw(graph);
    //     scene_texture.display();
    //     bloom_effect.apply(scene_texture, target);
    // }
    // else
    // {
        target.setView(view);
        target.draw(graph);
    // }
}
