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

namespace me = magic_enum;

world::world(
    state::stack::context_t& context)
    : target{context.window}
    , sound{context.sound}
    , heroes{context.players.size(), hero{nullptr, 3}}
    , n_pills{0}
    , immovables{{}}
    , enemy_mode_{entity::enemy::mode::scatter}
    , enemy_mode_timer{sf::seconds(7.f)}
    , done_timer{sf::seconds(3)}
{
    handle_size_changed({(unsigned int)target.getSize().x, (unsigned int)target.getSize().y});

    for(size_t i = 0; i != context.players.size(); ++i)
    {
        heroes[i].maker = context.players[i]->hero_maker();
    }

    build_scene();
}

commands_t& world::commands()
{
    return commands_;
}

bool world::players_alive() const
{
    return std::any_of(heroes.begin(), heroes.end(), [](auto const& h){ return h.lives > 0; });
}

bool world::players_done() const
{
    return n_pills == 0 || done_timer <= sf::Time::Zero;
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
    unsigned int const scoreboard_height = event.height * 0.05f;  // 5% of the window height.
    unsigned int const lifeboard_height = scoreboard_height;
    unsigned int const clearance = 20;

    auto const scale = std::min((static_cast<float>(event.height - scoreboard_height - lifeboard_height - clearance * 4) / (level::height * level::tile_size)),
                                (static_cast<float>(event.width - clearance * 2) / (level::width * level::tile_size)));

    auto const center_x = std::max(20.f, (event.width - level::width * level::tile_size * scale) / 2);

    scoreboard_.setPosition(center_x, clearance);
    scoreboard_.set_size(scoreboard_height, scale * level::width * level::tile_size);

    playground.setPosition(center_x, scoreboard_height + clearance);
    playground.setScale(scale, scale);

    lifeboard_.setPosition(center_x, event.height - lifeboard_height - clearance);
    lifeboard_.setScale(scale, scale);
}

void world::build_scene()
{
    // Read information of the first level.
    level::wall_texture_offsets wall_texture_offsets;
    level::wall_rotations wall_rotations;
    std::tie(level_info, wall_texture_offsets, wall_rotations) = read_level("assets/levels/1.txt");

    // Create a sound player.
    playground.attach<scene::sound_t>(sound);

    // Create layers.
    layers[me::enum_integer(layer::id::maze)] = playground.attach<layer::maze>();
    layers[me::enum_integer(layer::id::items)] = playground.attach<layer::items>();
    layers[me::enum_integer(layer::id::characters)] = playground.attach<layer::characters>();
    layers[me::enum_integer(layer::id::projectiles)] = playground.attach<layer::projectiles>();
    layers[me::enum_integer(layer::id::pipes)] = playground.attach<layer::pipes>();
    layers[me::enum_integer(layer::id::animations)] = playground.attach<layer::animations>();

    auto *m = layers[me::enum_integer(layer::id::maze)]->attach<maze>();
    m->layout(wall_texture_offsets, wall_rotations);

    // Get the rectangle coordinates of the ghost house.
    sf::IntRect ghost_house{};
    for(size_t r = 0; r != level::height; ++r)
    {
        for(size_t c = 0; c != level::width; ++c)
        {
            if(level_info[r][c] == 'h')
            {
                if(ghost_house.left == 0)
                {
                    ghost_house.left = c * level::tile_size;
                    ghost_house.top = r * level::tile_size;
                }
                else
                {
                    ghost_house.width = (c + 1) * level::tile_size - ghost_house.left;
                    ghost_house.height = (r + 1) * level::tile_size - ghost_house.top;
                }
            }
        }
    }

    // Create entities as the level dictates.
    for(size_t r = 0; r != level::height; ++r)
    {
        for(size_t c = 0; c != level::width; ++c)
        {
            entity::entity *e = nullptr;
            switch(level_info[r][c])
            {
                case '.':
                    e = layers[me::enum_integer(layer::id::items)]->attach<entity::power_up::coin>();
                    immovables[r][c] = e;
                    ++n_pills;
                    break;
                case 'f':
                    e = layers[me::enum_integer(layer::id::items)]->attach<entity::power_up::flower>();
                    immovables[r][c] = e;
                    break;
                case 'g':
                    {
                        auto *g = layers[me::enum_integer(layer::id::characters)]->attach<entity::goomba>(ghost_house);
                        if(!ghost_house.contains(c * level::tile_size, r * level::tile_size))
                        {
                            g->behave(entity::enemy::mode::scatter);
                        }
                        e = g;
                        e->throttle(1.f);
                    }
                    break;
                case 'm':
                    e = layers[me::enum_integer(layer::id::items)]->attach<entity::power_up::mushroom>();
                    immovables[r][c] = e;
                    break;
                case 'p':
                    e = layers[me::enum_integer(layer::id::pipes)]->attach<entity::pipe>();
                    immovables[r][c] = e;

                    if(c == 0)
                    {
                        e->setRotation(180);
                    }
                    break;
                case 'x':
                    {
                        auto h = heroes[0].maker();
                        e = heroes[0].hero_ = h.get();
                        layers[me::enum_integer(layer::id::characters)]->attach(std::move(h));

                        heroes[0].spawn_point = {(float)c * level::tile_size + level::half_tile_size, (float)r * level::tile_size + level::half_tile_size};
                    }
                    break;
                case 'y':
                    if(heroes.size() >= 2)
                    {
                        auto h = heroes[1].maker();
                        e = heroes[1].hero_ = h.get();
                        layers[me::enum_integer(layer::id::characters)]->attach(std::move(h));

                        heroes[1].hero_->head(direction::left);
                        heroes[1].spawn_point = {(float)c * level::tile_size + level::half_tile_size, (float)r * level::tile_size + level::half_tile_size};
                    }
                    break;
            }

            if(e)
            {
                e->setPosition(c * level::tile_size + level::half_tile_size, r * level::tile_size + level::half_tile_size);
            }
        }
    }

    // Pass information to the lifeboard.
    for(int i = 0; i != heroes.size(); ++i)
    {
        lifeboard_[i].lives = heroes[i].lives;
        lifeboard_[i].sprite = heroes[i].hero_->default_sprite();

        if(i % 2)
        {
            lifeboard_[i].sprite.setScale(-1, 1);
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

    // Detect collisions between the heroes and power-ups and pipes.
    for(auto const& hero : heroes)
    {
        if(hero.hero_)
        {
            size_t const r = hero.hero_->getPosition().y / level::tile_size, c = hero.hero_->getPosition().x / level::tile_size;
            if(auto* const immovable = immovables[r][c]; immovable && hero.hero_->collides(immovable))
            {
                collisions.insert(std::minmax<scene::node*>(hero.hero_, immovable));
            }
        }
    }

    // Detect collisions between characters and...
    for(auto* const character : layers[me::enum_integer(layer::id::characters)]->children())
    {
        //  ...projectiles.
        for(auto* const projectile : layers[me::enum_integer(layer::id::projectiles)]->children())
        {
            if(character->collides(projectile))
            {
                collisions.insert(std::minmax<scene::node*>(character, projectile));
            }
        }

        // ...other characters.
        for(auto* const other : layers[me::enum_integer(layer::id::characters)]->children())
        {
            if(character != other && character->collides(other))
            {
                collisions.insert(std::minmax<scene::node*>(character, other));
            }
        }
    }

    for(auto const& collision : collisions)
    {
        if(auto [hero, enemy] = match<entity::hero, entity::enemy>(collision); hero && enemy)
        {
            spdlog::info("Hero got hit by an enemy!");

            if(!hero->immune() && !enemy->immune())
            {
                hero->hit();
                if(hero->remove)
                {
                    sound.play(resources::sound_effect::short_die);

                    size_t const h = std::distance(heroes.begin(), std::find_if(heroes.begin(), heroes.end(), [hero_ = hero](auto const& h){ return hero_ == h.hero_;})); // clang bug workaround. c.f. https://stackoverflow.com/questions/67883701/structured-binding-violations

                    heroes[h].hero_ = nullptr;
                    if((lifeboard_[h].lives = --heroes[h].lives))
                    {
                        heroes[h].spawn_timer = sf::seconds(3);
                    }
                }
            }
        }
        else if(auto [hero, projectile] = match<entity::hero, entity::projectile>(collision); hero && projectile)
        {
            if(!projectile->remove)
            {
                spdlog::info("Hero got hit by a projectile!");

                projectile->hit();

                if(!hero->immune())
                {
                    hero->hit();

                    if(hero->remove)
                    {
                        sound.play(resources::sound_effect::short_die);

                        size_t const h = std::distance(heroes.begin(), std::find_if(heroes.begin(), heroes.end(), [hero_ = hero](auto const& h){ return hero_ == h.hero_;})); // clang bug workaround. c.f. https://stackoverflow.com/questions/67883701/structured-binding-violations

                        heroes[h].hero_ = nullptr;
                        if((lifeboard_[h].lives = --heroes[h].lives))
                        {
                            heroes[h].spawn_timer = sf::seconds(3);
                        }
                    }
                }
            }
        }
        if(auto [enemy, fireball] = match<entity::enemy, entity::fireball>(collision); enemy && fireball)
        {
            if(!enemy->immune() && !fireball->remove)
            {
                enemy->hit();
                fireball->hit();

                sound.play(resources::sound_effect::kick);
            }
        }
        else if(auto [hero, power_up] = match<entity::hero, entity::power_up::power_up>(collision); hero && power_up)
        {
            if(!power_up->remove)
            {
                immovables[power_up->getPosition().y / level::tile_size][power_up->getPosition().x / level::tile_size] = nullptr;

                hero->pick_up(power_up);

                sound.play(power_up->sound_effect());

                if(auto const* coin = dynamic_cast<entity::power_up::coin*>(power_up))
                {
                    --n_pills;

                    size_t const h = std::distance(heroes.begin(), std::find_if(heroes.begin(), heroes.end(), [hero_ = hero](auto const& h){ return hero_ == h.hero_;})); // clang bug workaround. c.f. https://stackoverflow.com/questions/67883701/structured-binding-violations

                    scoreboard_.increase_score(h, 10);
                }
            }
        }
        else if(auto [hero, pipe] = match<entity::hero, entity::pipe>(collision); hero && pipe)
        {
            sound.play(resources::sound_effect::warp);
            if(pipe->getPosition().x == level::half_tile_size)
            {
                hero->setPosition((level::width - 1) * level::tile_size - level::half_tile_size, hero->getPosition().y);
            }
            else
            {
                hero->setPosition(1 * level::tile_size + level::half_tile_size, hero->getPosition().y);
            }
        }
    }
}

void world::update_hero(
    entity::hero *hero)
{
    // Control a hero's direction given his desired direction (steering), current direction (heading) and any close by walls.
    sf::Vector2i const position{(int)hero->getPosition().x, (int)hero->getPosition().y};
    auto const heading = hero->heading();
    auto const steering = hero->steering();

    if(position.x % level::tile_size >= 8 && position.x % level::tile_size <= 12 && position.y % level::tile_size >= 8 && position.y % level::tile_size <= 12)
    {
        // If the hero wants to change direction and he can, let him.
        if((steering != heading || hero->speed() == 0.f) &&
           ((steering == direction::right  && !utility::any_of(level_info[position.y / level::tile_size][position.x / level::tile_size + 1], '0', '1', '2', '3', 'd')) ||
            (steering == direction::left   && !utility::any_of(level_info[position.y / level::tile_size][position.x / level::tile_size - 1], '0', '1', '2', '3', 'd')) ||
            (steering == direction::down   && !utility::any_of(level_info[position.y / level::tile_size + 1][position.x / level::tile_size], '0', '1', '2', '3', 'd')) ||
            (steering == direction::up     && !utility::any_of(level_info[position.y / level::tile_size - 1][position.x / level::tile_size], '0', '1', '2', '3', 'd'))))
        {
            spdlog::info("Changing heading at coordinates [{}, {}] to [{}]", position.x, position.y, me::enum_name(steering));
            hero->head(steering);
            hero->throttle(1.f);
            hero->setPosition((position.x / level::tile_size) * level::tile_size + level::half_tile_size, (position.y / level::tile_size) * level::tile_size + level::half_tile_size);
        }
        // Else if the hero is crusing along and he's about to face a wall, stop him.
        else if(hero->speed() != 0.f &&
                !((heading == direction::left || heading == direction::right) && utility::any_of(level_info[position.y / level::tile_size][position.x / level::tile_size], 'p')) &&
                ((heading == direction::right    && utility::any_of(level_info[position.y / level::tile_size][position.x / level::tile_size + 1], '0', '1', '2', '3', 'd')) ||
                 (heading == direction::left     && utility::any_of(level_info[position.y / level::tile_size][position.x / level::tile_size - 1], '0', '1', '2', '3', 'd')) ||
                 (heading == direction::down     && utility::any_of(level_info[position.y / level::tile_size + 1][position.x / level::tile_size], '0', '1', '2', '3', 'd')) ||
                 (heading == direction::up       && utility::any_of(level_info[position.y / level::tile_size - 1][position.x / level::tile_size], '0', '1', '2', '3', 'd'))))
        {
            spdlog::info("Hit a wall at coordinates [{}, {}] heading [{}]", position.x, position.y, me::enum_name(heading));
            hero->throttle(0.f);
        }
        // Else we let him cruise along.
    }
}

void world::update_fireballs()
{
    // If a fireball hits a wall or a pipe, remove it.
    for(auto* const projectile : layers[me::enum_integer(layer::id::projectiles)]->children())
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
    if(std::all_of(heroes.begin(), heroes.end(), [](auto const& h){ return h.hero_ == nullptr; }))
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

    for(auto* const character : layers[me::enum_integer(layer::id::characters)]->children())
    {
        if(auto* enemy = dynamic_cast<entity::enemy*>(character))
        {
            // enemy->behave(enemy_mode_);

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
                    std::vector<sf::Vector2f> heroes_positions;
                    if(heroes[0].hero_)
                    {
                        heroes_positions.push_back(heroes[0].hero_->getPosition());
                    }
                    if(heroes.size() > 1 && heroes[1].hero_)
                    {
                        heroes_positions.push_back(heroes[1].hero_->getPosition());
                    }

                    // Change heading given chasing strategy.
                    enemy->head(enemy->fork(heroes_positions, paths));

                    // Nudge it along so it doesn't get to redecide immediately...
                    enemy->nudge(2.f);

                    spdlog::info("{} decided to go {}", enemy->name(), me::enum_name(enemy->heading()));
                }
                // Else, if it hit a wall, follow along the path.
                else if(paths.begin()->first != heading)
                {
                    enemy->head(paths.begin()->first);

                    // Nudge it along so it doesn't get to redecide immediately...
                    enemy->nudge(2.f);

                    spdlog::info("{} is turning {}", enemy->name(), me::enum_name(enemy->heading()));
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

    // Update the heroes' movements or respawn them if the time has come.
    for(auto& hero : heroes)
    {
        if(hero.hero_)
        {
            update_hero(hero.hero_);
        }
        else if(hero.lives > 0 && (hero.spawn_timer -= dt) <= sf::Time::Zero)
        {
            auto h = hero.maker();
            hero.hero_ = h.get();
            hero.hero_->setPosition(hero.spawn_point);

            layers[me::enum_integer(layer::id::characters)]->attach(std::move(h));
        }
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
    for(auto& node : *layers[me::enum_integer(layer::id::animations)])
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

    if(!players_alive())
    {
        if(done_timer == sf::seconds(3))
        {
            sound.play(resources::sound_effect::die);
        }
        done_timer -= dt;

        return;
    }
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
        target.draw(scoreboard_);
        target.draw(playground);
        target.draw(lifeboard_);
    // }
}
