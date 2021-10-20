#include "world.h"

#include "background.h"
#include "command.h"
#include "configuration.h"
// #include "effects/bloom.h"
// #include "effects/post_effect.h"
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
    // Create a sound player.
    playground.attach<scene::sound_t>(sound);

    // Create layers.
    layers[me::enum_integer(layer::id::background)] = playground.attach<layer::background>();
    layers[me::enum_integer(layer::id::maze)] = playground.attach<layer::maze>();
    layers[me::enum_integer(layer::id::items)] = playground.attach<layer::items>();
    layers[me::enum_integer(layer::id::characters)] = playground.attach<layer::characters>();
    layers[me::enum_integer(layer::id::projectiles)] = playground.attach<layer::projectiles>();
    layers[me::enum_integer(layer::id::pipes)] = playground.attach<layer::pipes>();
    layers[me::enum_integer(layer::id::animations)] = playground.attach<layer::animations>();

    layers[me::enum_integer(layer::id::background)]->attach<background>();
    maze_ = layers[me::enum_integer(layer::id::maze)]->attach<maze>("assets/levels/1.txt");

    // Create entities as the level dictates.
    for(int r = 0; r != level::height; ++r)
    {
        for(int c = 0; c != level::width; ++c)
        {
            entity::entity *e = nullptr;
            switch((*maze_)[{c, r}])
            {
                case '.':   // Simple coin
                    e = layers[me::enum_integer(layer::id::items)]->attach<entity::super_mario::coin>();
                    immovables[r][c] = e;
                    ++n_pills;
                    break;
                case 'p':   // Lesser power-up
                    e = layers[me::enum_integer(layer::id::items)]->attach<entity::super_mario::mushroom>();
                    immovables[r][c] = e;
                    break;
                case 'P':   // Greater power-up
                    e = layers[me::enum_integer(layer::id::items)]->attach<entity::super_mario::flower>();
                    immovables[r][c] = e;
                    break;
                case 'a':   // Level-1 enemy
                case 'b':
                case 'c':
                case 'd':
                    {
                        entity::enemy *p;
                        switch((*maze_)[{c, r}])
                        {
                            case 'a':
                                p = layers[me::enum_integer(layer::id::characters)]->attach<entity::super_mario::goomba>();
                                break;
                            case 'b':
                                p = layers[me::enum_integer(layer::id::characters)]->attach<entity::super_mario::koopa>();
                                break;
                            case 'c':
                                p = layers[me::enum_integer(layer::id::characters)]->attach<entity::super_mario::beetle>();
                                break;
                            case 'd':
                                p = layers[me::enum_integer(layer::id::characters)]->attach<entity::super_mario::hammer>();
                                break;
                        }
                        if(!maze_->ghost_house().contains(c, r))
                        {
                            p->behave(entity::enemy::mode::scatter);
                        }
                        e = p;
                        e->throttle(1.f);
                    }
                    break;
                case 'w':   // Pipe/warp
                    e = layers[me::enum_integer(layer::id::pipes)]->attach<entity::pipe>();
                    immovables[r][c] = e;

                    if(c == 0)
                    {
                        e->setRotation(180);
                    }
                    break;
                case 'x':   // First hero
                    {
                        auto h = heroes[0].maker();
                        e = heroes[0].hero_ = h.get();
                        layers[me::enum_integer(layer::id::characters)]->attach(std::move(h));

                        heroes[0].spawn_point = {(float)c * level::tile_size + level::half_tile_size, (float)r * level::tile_size + level::half_tile_size};
                    }
                    break;
                case 'y':   // Second hero
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
            if(!hero->immune() && !enemy->immune())
            {
                hero->hit();
                spdlog::info("Hero got hit by an enemy!");

                if(hero->dead())
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
                if(!hero->immune())
                {
                    spdlog::info("Hero got hit by a projectile!");
                    hero->hit();
                    projectile->hit();

                    if(hero->dead())
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
        if(auto [enemy, projectile] = match<entity::enemy, entity::friendly<entity::projectile>>(collision); enemy && projectile)
        {
            if(!enemy->immune() && !projectile->remove)
            {
                spdlog::info("Enemy got hit by a projectile!");
                enemy->hit();
                projectile->hit();

                sound.play(resources::sound_effect::kick);
            }
        }
        else if(auto [hero, power_up] = match<entity::hero, entity::power_up>(collision); hero && power_up)
        {
            if(!power_up->remove)
            {
                immovables[power_up->getPosition().y / level::tile_size][power_up->getPosition().x / level::tile_size] = nullptr;

                hero->pick_up(power_up);

                sound.play(power_up->sound_effect());

                if(auto const* coin = dynamic_cast<entity::super_mario::coin*>(power_up))
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

void world::update_enemies_behavior(
    sf::Time const dt)
{
    auto enemy_mode = enemy_mode_;

    // If no heroes are alive, scatter.
    if(std::all_of(heroes.begin(), heroes.end(), [](auto const& h){ return h.hero_ == nullptr; }))
    {
        enemy_mode = entity::enemy::mode::scatter;
        enemy_mode_timer = sf::seconds(7.f);
    }
    // Otherwise, switch mode if the timer has gone off.
    else if((enemy_mode_timer -= dt) <= sf::Time::Zero)
    {
        switch(enemy_mode)
        {
            case entity::enemy::mode::scatter:
                enemy_mode = entity::enemy::mode::chase;
                enemy_mode_timer = sf::seconds(20.f);
                break;
            case entity::enemy::mode::chase:
                enemy_mode = entity::enemy::mode::scatter;
                enemy_mode_timer = sf::seconds(7.f);
                break;
            default:
                break;
        }
    }

    if(enemy_mode != enemy_mode_)
    {
        enemy_mode_ = enemy_mode;

        for(auto* const character : layers[me::enum_integer(layer::id::characters)]->children())
        {
            if(auto* enemy = dynamic_cast<entity::enemy*>(character))
            {
                enemy->behave(enemy_mode_);
            }
        }
    }
}

void world::update(
    sf::Time const dt)
{
    // Respawn heroes if the time has come.
    for(auto& hero : heroes)
    {
        if(!hero.hero_ && hero.lives > 0 && (hero.spawn_timer -= dt) <= sf::Time::Zero)
        {
            auto h = hero.maker();
            hero.hero_ = h.get();
            hero.hero_->setPosition(hero.spawn_point);

            layers[me::enum_integer(layer::id::characters)]->attach(std::move(h));
        }
    }

    // Dispatch commands.
    while(!commands_.empty())
    {
        playground.on_command(commands_.front(), dt);
        commands_.pop();
    }

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

    // Deal with collisions.
    handle_collisions();

    // Switch between scattering and chasing.
    update_enemies_behavior(dt);

    // Update the entire playground.
    playground.update(dt, commands_);

    if(!players_alive())
    {
        if(done_timer == sf::seconds(3))
        {
            sound.play(resources::sound_effect::die);
        }
        done_timer -= dt;
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
