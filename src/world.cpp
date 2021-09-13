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
#include <map>
#include <memory>
#include <numeric>
#include <tuple>
#include <vector>

world::world(
    sf::RenderTarget& output_target,
    sound::player& sound)
    : target{output_target}
    , sound{sound}
    , lifeboard_{entity::mario::default_sprite(), entity::luigi::default_sprite()}
    , mario{nullptr}
    , luigi{nullptr}
    , mario_lives{3}
    , luigi_lives{3}
    , n_pills{0}
    , immovables{{}}
    , enemy_mode_{entity::enemy::mode::scatter}
    , enemy_mode_timer{sf::seconds(7.f)}
{
    handle_size_changed({(unsigned int)target.getSize().x, (unsigned int)target.getSize().y});

    build_scene();
}

commands_t& world::commands()
{
    return commands_;
}

bool world::players_alive() const
{
    return mario_lives > 0 || luigi_lives > 0;
}

bool world::players_done() const
{
    return !n_pills;
}

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

void world::handle_size_changed(
    sf::Event::SizeEvent const& event)
{
    target.setView(sf::View{sf::FloatRect{0, 0, (float)event.width, (float)event.height}});

    auto const scale = std::min((static_cast<float>(event.height - 100) / (level::height * level::tile_size)),
                                (static_cast<float>(event.width - 40) / (level::width * level::tile_size)));

    playground.setPosition(std::max(20.f, (event.width - level::width * level::tile_size * scale) / 2), 
                           std::max(50.f, (event.height - level::height * level::tile_size * scale) / 2));
    playground.setScale(scale, scale);

    lifeboard_.setPosition(std::max(20.f, (event.width - level::width * level::tile_size * scale) / 2),
                           std::min(playground.getPosition().y + level::height * level::tile_size * scale, event.height - 50.f));
    lifeboard_.setScale(scale, scale);
}

void world::build_scene()
{
    lifeboard_[0] = 3;
    lifeboard_[1] = 3;

    // Read information of the first level.
    level::wall_texture_offsets wall_texture_offsets;
    level::wall_rotations wall_rotations;
    std::tie(level_info, wall_texture_offsets, wall_rotations) = read_level("assets/levels/1.txt");

    // Create a sound player.
    playground.attach<scene::sound_t>(sound);

    // Create layers.
    layers[magic_enum::enum_integer(layer::id::maze)] = playground.attach<layer::maze>();
    layers[magic_enum::enum_integer(layer::id::items)] = playground.attach<layer::items>();
    layers[magic_enum::enum_integer(layer::id::characters)] = playground.attach<layer::characters>();
    layers[magic_enum::enum_integer(layer::id::projectiles)] = playground.attach<layer::projectiles>();
    layers[magic_enum::enum_integer(layer::id::pipes)] = playground.attach<layer::pipes>();
    layers[magic_enum::enum_integer(layer::id::animations)] = playground.attach<layer::animations>();

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
                    ++n_pills;
                    break;
                case 'f':
                    e = layers[magic_enum::enum_integer(layer::id::items)]->attach<entity::pickup::flower>();
                    immovables[r][c] = e;
                    break;
                case 'g':
                    e = layers[magic_enum::enum_integer(layer::id::characters)]->attach<entity::goomba>(sf::Vector2f{level::width * level::tile_size / 2, level::height * level::tile_size / 2});
                    e->throttle(1.f);
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
                    mario_spawn_x = c * level::tile_size + level::half_tile_size;
                    mario_spawn_y = r * level::tile_size + level::half_tile_size;
                    break;
                case 'y':
                    e = luigi = layers[magic_enum::enum_integer(layer::id::characters)]->attach<entity::luigi>();
                    luigi_spawn_x = c * level::tile_size + level::half_tile_size;
                    luigi_spawn_y = r * level::tile_size + level::half_tile_size;
                    break;
            }

            if(e)
            {
                e->setPosition(c * level::tile_size + level::half_tile_size, r * level::tile_size + level::half_tile_size);
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

void world::handle_collisions()
{
    std::set<std::pair<scene::node*, scene::node*>> collisions;

    // Detect collisions between the brothers and pickups and pipes.
    for(auto* const bro : std::initializer_list<entity::brother*>{mario, luigi})
    {
        if(bro)
        {
            size_t const r = bro->getPosition().y / level::tile_size, c = bro->getPosition().x / level::tile_size;
            if(auto* const immovable = immovables[r][c]; immovable && bro->collides(immovable))
            {
                collisions.insert(std::minmax<scene::node*>(bro, immovable));
            }
        }
    }

    // Detect collisions between characters and...
    for(auto* const character : layers[magic_enum::enum_integer(layer::id::characters)]->children())
    {
        //  ...projectiles.
        for(auto* const projectile : layers[magic_enum::enum_integer(layer::id::projectiles)]->children())
        {
            if(character->collides(projectile))
            {
                collisions.insert(std::minmax<scene::node*>(character, projectile));
            }
        }

        // ...other characters.
        for(auto* const other : layers[magic_enum::enum_integer(layer::id::characters)]->children())
        {
            if(character != other && character->collides(other))
            {
                collisions.insert(std::minmax<scene::node*>(character, other));
            }
        }
    }

    for(auto const& collision : collisions)
    {
        if(auto [brother, enemy] = match<entity::brother, entity::enemy>(collision); brother && enemy)
        {
            spdlog::info("Brother got hit by an enemy!");

            brother->hit();

            if(brother == mario)
            {
                mario = nullptr;
                lifeboard_[0] = --mario_lives;
                mario_spawn_timer = sf::seconds(3);
            }
            else
            {
                luigi = nullptr;
                lifeboard_[1] = --luigi_lives;
                luigi_spawn_timer = sf::seconds(3);
            }
        }
        else if(auto [brother, projectile] = match<entity::brother, entity::projectile>(collision); brother && projectile)
        {
            if(!projectile->remove)
            {
                spdlog::info("Brother got hit by a projectile!");

                brother->hit();
                projectile->hit();

                if(brother == mario)
                {
                    mario = nullptr;
                    lifeboard_[0] = --mario_lives;
                    mario_spawn_timer = sf::seconds(3);
                }
                else
                {
                    luigi = nullptr;
                    lifeboard_[1] = --luigi_lives;
                    luigi_spawn_timer = sf::seconds(3);
                }
            }
        }
        if(auto [enemy, fireball] = match<entity::enemy, entity::fireball>(collision); enemy && fireball)
        {
            if(!fireball->remove)
            {
                enemy->hit();
                fireball->hit();

                sound.play(resources::sound_effect::kick);
            }
        }
        else if(auto [brother, pickup] = match<entity::brother, entity::pickup::pickup>(collision); brother && pickup)
        {
            if(!pickup->remove)
            {
                pickup->remove = true;
                immovables[pickup->getPosition().y / level::tile_size][pickup->getPosition().x / level::tile_size] = nullptr;

                pickup->apply(*brother);
                sound.play(pickup->sound_effect());

                if(auto const* coin = dynamic_cast<entity::pickup::coin*>(pickup))
                {
                    --n_pills;
                }
            }
        }
        else if(auto [brother, pipe] = match<entity::brother, entity::pipe>(collision); brother && pipe)
        {
            sound.play(resources::sound_effect::warp);
            if(pipe->getPosition().x == level::half_tile_size)
            {
                brother->setPosition((level::width - 1) * level::tile_size - level::half_tile_size, brother->getPosition().y);
            }
            else
            {
                brother->setPosition(1 * level::tile_size + level::half_tile_size, brother->getPosition().y);
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

void world::update_brother(
    entity::brother *bro)
{
    // Control a brother's direction given his desired direction (steering), current direction (heading) and any close by walls.
    sf::Vector2i const position{(int)bro->getPosition().x, (int)bro->getPosition().y};
    auto const heading = bro->heading();
    auto const steering = bro->steering();

    if(position.x % level::tile_size >= 8 && position.x % level::tile_size <= 12 && position.y % level::tile_size >= 8 && position.y % level::tile_size <= 12)
    {
        // If the brother wants to change direction and he can, let him.
        if((steering != heading || bro->speed() == 0.f) &&
           ((steering == direction::right  && !utility::any_of(level_info[position.y / level::tile_size][position.x / level::tile_size + 1], '0', '1', '2', '3')) ||
            (steering == direction::left   && !utility::any_of(level_info[position.y / level::tile_size][position.x / level::tile_size - 1], '0', '1', '2', '3')) ||
            (steering == direction::down   && !utility::any_of(level_info[position.y / level::tile_size + 1][position.x / level::tile_size], '0', '1', '2', '3')) ||
            (steering == direction::up     && !utility::any_of(level_info[position.y / level::tile_size - 1][position.x / level::tile_size], '0', '1', '2', '3'))))
        {
            spdlog::info("Changing heading at coordinates [{}, {}] to [{}]", position.x, position.y, magic_enum::enum_name(steering));
            bro->head(steering);
            bro->throttle(1.f);
            bro->setPosition((position.x / level::tile_size) * level::tile_size + level::half_tile_size, (position.y / level::tile_size) * level::tile_size + level::half_tile_size);
        }
        // Else if the brother is crusing along and he's about to face a wall, stop him.
        else if(bro->speed() != 0.f &&
                !((heading == direction::left || heading == direction::right) && utility::any_of(level_info[position.y / level::tile_size][position.x / level::tile_size], 'p')) &&
                ((heading == direction::right    && utility::any_of(level_info[position.y / level::tile_size][position.x / level::tile_size + 1], '0', '1', '2', '3')) ||
                 (heading == direction::left     && utility::any_of(level_info[position.y / level::tile_size][position.x / level::tile_size - 1], '0', '1', '2', '3')) ||
                 (heading == direction::down     && utility::any_of(level_info[position.y / level::tile_size + 1][position.x / level::tile_size], '0', '1', '2', '3')) ||
                 (heading == direction::up       && utility::any_of(level_info[position.y / level::tile_size - 1][position.x / level::tile_size], '0', '1', '2', '3'))))
        {
            spdlog::info("Hit a wall at coordinates [{}, {}] heading [{}]", position.x, position.y, magic_enum::enum_name(heading));
            bro->throttle(0.f);
        }
        // Else we let him cruise along.
    }
}

void world::update_fireballs()
{
    // If a fireball hits a wall or a pipe, remove it.
    for(auto* const projectile : layers[magic_enum::enum_integer(layer::id::projectiles)]->children())
    {
        auto const r = projectile->getPosition().y / level::tile_size, c = projectile->getPosition().x / level::tile_size;
        if(utility::any_of(level_info[r][c], '0', '1', '2', '3', 'p'))
        {
            if(auto* fireball = dynamic_cast<entity::fireball*>(projectile))
            {
                // Bring it closer to the wall.
                fireball->nudge(7.f);

                fireball->hit();

                sound.play(resources::sound_effect::bump);
            }
        }
    }
}

void world::update_enemies(
    sf::Time const dt)
{
    if(!mario && !luigi)
    {
        enemy_mode_ = entity::enemy::mode::scatter;
        enemy_mode_timer = sf::seconds(7.f);
    }
    else if((enemy_mode_timer -= dt) <= sf::Time::Zero)
    {
        switch(enemy_mode_)
        {
            case entity::enemy::mode::scatter:
                enemy_mode_ = entity::enemy::mode::chase;
                enemy_mode_timer = sf::seconds(20.f);
                break;
            case entity::enemy::mode::chase:
                enemy_mode_ = entity::enemy::mode::scatter;
                enemy_mode_timer = sf::seconds(7.f);
                break;
            default:
                break;
        }
    }

    for(auto* const character : layers[magic_enum::enum_integer(layer::id::characters)]->children())
    {
        if(auto* enemy = dynamic_cast<entity::enemy*>(character))
        {
            enemy->behave(enemy_mode_);

            sf::Vector2i const position{(int)enemy->getPosition().x, (int)enemy->getPosition().y};
            if(position.x % level::tile_size >= 9 && position.x % level::tile_size <= 11 && position.y % level::tile_size >= 9 && position.y % level::tile_size <= 11)
            {
                auto const heading = enemy->heading();

                std::map<direction, sf::Vector2f> paths;
                if(heading != direction::left && !utility::any_of(level_info[position.y / level::tile_size][position.x / level::tile_size + 1], '0', '1', '2', '3', 'p'))
                {
                    paths[direction::right] = {(float)position.x + level::tile_size, (float)position.y};
                }
                if(heading != direction::right && !utility::any_of(level_info[position.y / level::tile_size][position.x / level::tile_size - 1], '0', '1', '2', '3', 'p'))
                {
                    paths[direction::left] = {(float)position.x - level::tile_size, (float)position.y};
                }
                if(heading != direction::up && !utility::any_of(level_info[position.y / level::tile_size + 1][position.x / level::tile_size], '0', '1', '2', '3', 'p'))
                {
                    paths[direction::down] = {(float)position.x, (float)position.y + level::tile_size};
                }
                if(heading != direction::down && !utility::any_of(level_info[position.y / level::tile_size - 1][position.x / level::tile_size], '0', '1', '2', '3', 'p'))
                {
                    paths[direction::up] = {(float)position.x, (float)position.y - level::tile_size};
                }

                // In case it's in a pipe...
                if(heading == direction::left && level_info[position.y / level::tile_size][position.x / level::tile_size - 1] == 'p')
                {
                    paths[direction::right] = {(float)position.x + level::tile_size, (float)position.y};
                }
                else if(heading == direction::right && level_info[position.y / level::tile_size][position.x / level::tile_size + 1] == 'p')
                {
                    paths[direction::left] = {(float)position.x - level::tile_size, (float)position.y};
                }

                // If it is at an intersection, ask it for a direction.
                if(paths.size() >= 2)
                {
                    std::vector<sf::Vector2f> brothers_positions;
                    if(mario)
                    {
                        brothers_positions.push_back(mario->getPosition());
                    }
                    if(luigi)
                    {
                        brothers_positions.push_back(luigi->getPosition());
                    }

                    // Change heading given chasing strategy.
                    enemy->head(enemy->fork(brothers_positions, paths));

                    // Nudge it along so it doesn't get to redecide immediately...
                    enemy->nudge(2.f);

                    spdlog::info("{} decided to go {}", enemy->name(), magic_enum::enum_name(enemy->heading()));
                }
                // Else, if it hit a wall, follow along the path.
                else if(paths.begin()->first != heading)
                {
                    enemy->head(paths.begin()->first);

                    // Nudge it along so it doesn't get to redecide immediately...
                    enemy->nudge(2.f);

                    spdlog::info("{} is turning {}", enemy->name(), magic_enum::enum_name(enemy->heading()));
                }
                // Else, let it cruise along.
            }
        }
    }
}

void world::update(
    sf::Time const dt)
{
    // Dispatch commands.
    while(!commands_.empty())
    {
        playground.on_command(commands_.front(), dt);
        commands_.pop();
    }

    // Update the brothers' movements or respawn them if the time has come.
    if(mario)
    {
        update_brother(mario);
    }
    else if(mario_lives > 0 && (mario_spawn_timer -= dt) <= sf::Time::Zero)
    {
        mario = layers[magic_enum::enum_integer(layer::id::characters)]->attach<entity::mario>();
        mario->setPosition(mario_spawn_x, mario_spawn_y);
    }

    if(luigi)
    {
        update_brother(luigi);
    }
    else if(luigi_lives > 0 && (luigi_spawn_timer -= dt) <= sf::Time::Zero)
    {
        luigi = layers[magic_enum::enum_integer(layer::id::characters)]->attach<entity::luigi>();
        luigi->setPosition(luigi_spawn_x, luigi_spawn_y);
    }

    // Update fireballs' movements.
    update_fireballs();

    // Update enemies' movements.
    update_enemies(dt);

    // Deal with collision.
    handle_collisions();

    // Update the entire playground.
    playground.update(dt, commands_);

    // Tag all unviewable animations to be removed.
    for(auto& node : playground)
    {
        auto const position = node.getInverseTransform().transformPoint(node.getPosition());
        if(position.x > target.getSize().x ||
           position.y > target.getSize().y)
        {
            node.remove = true;
        }
    }

    // Remove all destroyed entities.
    playground.sweep_removed();

    // Remove played sounds.
    sound.remove_stopped();


}

void world::draw()
{
    // if(effect::post_effect::supported())
    // {
    //     scene_texture.clear();
    //     scene_texture.setView(view);
    //     scene_texture.draw(playground);
    //     scene_texture.display();
    //     bloom_effect.apply(scene_texture, target);
    // }
    // else
    // {
        target.draw(playground);
        target.draw(lifeboard_);
    // }
}
