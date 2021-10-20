#include "entity/enemy.h"

#include "configuration.h"
#include "direction.h"
#include "entity/character.h"
#include "entity/hero.h"
#include "level.h"
#include "resources.h"
#include "scene.h"
#include "sprite.h"
#include "tomlpp.h"
#include "utility.h"

#include <magic_enum.hpp>
#include <SFML/Graphics.hpp>
#include <spdlog/spdlog.h>

#include <cmath>
#include <functional>
#include <map>
#include <memory>
#include <string_view>
#include <vector>

namespace entity
{

namespace me = magic_enum;

sf::Vector2i random_home_corner(
    sf::IntRect const& home)
{
    return {home.left + utility::random(1) * (home.width - 1),
            home.top + utility::random(1) * (home.height - 1)};
}

sf::Vector2i to_maze_coordinates(
    sf::Vector2f const& position)
{
    return {(int)position.x / level::tile_size, (int)position.y / level::tile_size};
}

sf::Vector2f to_playground_coordinates(
    sf::Vector2i const& position)
{
    return {(float)position.x * level::tile_size, (float)position.y * level::tile_size};
}

enemy::enemy(
    sprite sprite_,
    int const max_speed,
    direction const heading_)
    : hostile<character>{sprite_, max_speed, heading_}
    , requested_mode_{mode::scatter}
    , maze_{nullptr}
    , healed{true}
{}

void enemy::hit()
{
    behave(mode::dead);

    update_sprite();
}

bool enemy::immune() const
{
    return current_mode_ == mode::dead;
}

void enemy::behave(
    mode const requested_mode)
{
    if(current_mode_ == requested_mode ||
       (current_mode_ == mode::dead && !healed) ||
       (current_mode_ == mode::confined && confinement > sf::Time::Zero))
    {
        requested_mode_ = requested_mode;
        return;
    }

    spdlog::info("{} changing mode from {} to {}.", name(), me::enum_name(current_mode_), me::enum_name(requested_mode));

    // As per the Pac-Man rule, ghosts reverse direction when switching from `chase` or `scatter`.
    if(current_mode_ == mode::chase ||
       current_mode_ == mode::scatter)
    {
        head(~heading());
    }

    current_mode_ = requested_mode;

    switch(current_mode_)
    {
        case mode::confined:
            assert(!"Do not manually change behavior to 'confined'. This mode is set form the start and never reoccurs.");
            break;
        case mode::chase:
            throttle(1.f);
            break;
        case mode::dead:
            healed = false;
            target_ = random_home_corner(maze_->ghost_house());
            throttle(2.f);
            break;
        case mode::frightened:
            break;
        case mode::scatter:
            target_ = scatter_corner_;
            spdlog::info("{} targeting random corner [{}, {}].", name(), target_.x, target_.y);
            throttle(1.f);
            break;
    }

    update_sprite();
}

void enemy::update_sprite()
{
    character::update_sprite();
}

void enemy::update_self(
    sf::Time const& dt,
    commands_t& commands)
{
    if(!maze_)
    {
        commands.push(make_command(std::function{[this](maze& m, sf::Time const&)
        {
            maze_ = &m;
        }}));
    }
    else
    {
        sf::Vector2f const position = getPosition();
        sf::Vector2f const future_position = position + to_vector2f(heading_) * (max_speed * throttle_) * dt.asSeconds();

        // If the position delta between now and then crosses the middle of a tile...
        if((future_position.x > position.x && fmod(position.x, level::tile_size) <= level::half_tile_size && fmod(future_position.x, level::tile_size) > level::half_tile_size) ||
           (future_position.x < position.x && fmod(position.x, level::tile_size) >= level::half_tile_size && fmod(future_position.x, level::tile_size) < level::half_tile_size) ||
           (future_position.y > position.y && fmod(position.y, level::tile_size) <= level::half_tile_size && fmod(future_position.y, level::tile_size) > level::half_tile_size) ||
           (future_position.y < position.y && fmod(position.y, level::tile_size) >= level::half_tile_size && fmod(future_position.y, level::tile_size) < level::half_tile_size))
        {
            std::set<direction> paths;
            auto const around = maze_->around({(int)position.x / level::tile_size, (int)position.y / level::tile_size});
            if(heading() != direction::left && utility::any_of(maze::to_structure(around.at(direction::right)), maze::structure::path, maze::structure::door))
            {
                paths.insert(direction::right);
            }
            if(heading() != direction::right && utility::any_of(maze::to_structure(around.at(direction::left)), maze::structure::path, maze::structure::door))
            {
                paths.insert(direction::left);
            }
            if(heading() != direction::up && utility::any_of(maze::to_structure(around.at(direction::down)), maze::structure::path, maze::structure::door))
            {
                paths.insert(direction::down);
            }
            if(heading() != direction::down && utility::any_of(maze::to_structure(around.at(direction::up)), maze::structure::path, maze::structure::door))
            {
                paths.insert(direction::up);
            }

            // In case it's in a pipe...
            if(heading() == direction::left && maze::to_structure(around.at(direction::left)) == maze::structure::pipe)
            {
                paths.insert(direction::right);
            }
            else if(heading() == direction::right && maze::to_structure(around.at(direction::right)) == maze::structure::pipe)
            {
                paths.insert(direction::left);
            }

            // If it is at an intersection, ask it for a direction.
            if(paths.size() >= 2)
            {
                // Pick and adjust heading given current target.
                sf::Vector2i const start{(int)position.x / level::tile_size, (int)position.y / level::tile_size};
                auto const r = maze_->route(start, target_);

                // Head towards the best route but only if it's not backtracking because that is not allowed.
                if(r != ~heading() && r != direction::none)
                {
                    head(r);
                }
                else
                {
                    head(*paths.begin());
                }

                spdlog::info("{} decided to go {}", name(), me::enum_name(heading()));
            }
            // Else, if it hit a wall, follow along the path.
            else if(*paths.begin() != heading())
            {
                head(*paths.begin());

                spdlog::info("{} is turning {}", name(), me::enum_name(heading()));
            }
            // Else, let it cruise along.
        }
    }

    if(current_mode_ == mode::confined)
    {
        if(!maze_)
        {}
        else if((confinement -= dt) <= sf::Time::Zero ||
                !maze_->ghost_house().contains(to_maze_coordinates(getPosition())))
        {
            confinement = sf::Time::Zero;

            behave(requested_mode_);
        }
        else if(target_ == sf::Vector2i{0, 0} ||
                to_maze_coordinates(getPosition()) == target_)
        {
            target_ = random_home_corner(maze_->ghost_house());
        }
    }
    else if(current_mode_ == mode::dead &&
            maze_->ghost_house().contains(to_maze_coordinates(getPosition())))
    {
        healed = true;

        behave(requested_mode_);
    }

    character::update_self(dt, commands);
}

void enemy::draw_self(
    sf::RenderTarget& target,
    sf::RenderStates states) const
{
    // Show this enemy's target as an 'X'.
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::T))
    {
        for(auto const angle : {45.f, 45.f + 90.f})
        {
            sf::RectangleShape x{{25, 5}};
            utility::center_origin(x);
            x.setRotation(angle);
            x.setFillColor(((current_mode_ == mode::chase) ? sf::Color::Red : sf::Color::Yellow) * sf::Color{255, 255, 255, 128});
            x.setPosition({(float)target_.x * level::tile_size + level::half_tile_size, (float)target_.y * level::tile_size + level::half_tile_size});

            target.draw(x, {parent->world_transform()});
        }
    }

    return character::draw_self(target, states);
}

follower::follower(
    sprite sprite_,
    int const max_speed)
    : enemy{sprite_, max_speed}
{
    current_mode_ = mode::scatter;
    target_ = scatter_corner_ = {1, 1};
    confinement = sf::Time::Zero;
}

void follower::update_self(
    sf::Time const& dt,
    commands_t& commands)
{
    commands.push(make_command(std::function{[this](layer::characters& characters, sf::Time const&)
    {
        if(current_mode_ == mode::chase)
        {
            // Find the closest hero and target it.
            std::vector<sf::Vector2f> heroes_positions;

            for(auto* const character : characters.children())
            {
                if(auto const* h = dynamic_cast<hero const*>(character))
                {
                    heroes_positions.push_back(h->getPosition());
                }
            }

            if(heroes_positions.size())
            {
                auto const closest = std::min_element(heroes_positions.begin(), heroes_positions.end(), [this](auto const& p1, auto const& p2)
                {
                    return utility::length(getPosition() - p1) < utility::length(getPosition() - p2);
                });

                target_ = to_maze_coordinates(*closest);
            }

            spdlog::info("{} targeting following [{}, {}]", name(), target_.x, target_.y);
        }
    }}));

    enemy::update_self(dt, commands);
}

ahead::ahead(
    sprite sprite_,
    int const max_speed)
    : enemy{sprite_, max_speed}
{
    current_mode_ = mode::confined;
    scatter_corner_ = {level::width - 2, 1};
    confinement = sf::seconds(3);
}

void ahead::update_self(
    sf::Time const& dt,
    commands_t& commands)
{
    commands.push(make_command(std::function{[this](layer::characters& characters, sf::Time const&)
    {
        if(current_mode_ == mode::chase)
        {
            // Find the closest hero.
            std::vector<hero const*> heroes;

            for(auto* const character : characters.children())
            {
                if(auto const* h = dynamic_cast<hero const*>(character))
                {
                    heroes.push_back(h);
                }
            }

            if(heroes.size())
            {
                auto const closest = *std::min_element(heroes.begin(), heroes.end(), [this](auto const& h1, auto const& h2)
                {
                    return utility::length(getPosition() - h1->getPosition()) < utility::length(getPosition() - h2->getPosition());
                });

                // Start targetting from the hero's position...
                target_ = to_maze_coordinates(closest->getPosition());

                // ...then guess its position four tiles ahead.
                auto h = closest->heading();
                for(int i = 0; i != 4; ++i)
                {
                    // Get the nature of the tiles around the target.
                    auto around = maze_->around(target_);

                    // Remove the tile that would be opposite of the heading.
                    around.erase(~h);

                    // If there's path straight ahead, pick that...
                    if(maze::to_structure(around.at(h)) == maze::structure::path)
                    {
                        target_ = target_ + to_vector2i(h);
                    }
                    // ...else pick the first other possible path.
                    else
                    {
                        // Remove the choice that's ahead since ahead is not a path.
                        around.erase(h);

                        // If the first choice is not a path, remove it.
                        if(maze::to_structure(around.begin()->second) != maze::structure::path)
                        {
                            around.erase(around.begin());
                        }

                        // We're left with the first choice being a path for sure.
                        h = around.begin()->first;
                        target_ = target_ + to_vector2i(h);
                    }
                }
            }

            spdlog::info("{} targeting ahead [{}, {}]", name(), target_.x, target_.y);
        }
    }}));

    enemy::update_self(dt, commands);
}

axis::axis(
    sprite sprite_,
    int const max_speed)
    : enemy{sprite_, max_speed}
{
    current_mode_ = mode::confined;
    scatter_corner_ = {1, level::height - 2};
    confinement = sf::seconds(10);
}

void axis::update_self(
    sf::Time const& dt,
    commands_t& commands)
{
    commands.push(make_command(std::function{[this](layer::characters& characters, sf::Time const&)
    {
        if(current_mode_ == mode::chase)
        {
            // Find the closest hero and the follower.
            std::vector<hero const*> heroes;

            for(auto* const character : characters.children())
            {
                if(auto const* c = dynamic_cast<hero const*>(character))
                {
                    heroes.push_back(c);
                }
                else if(auto const* c = dynamic_cast<follower const*>(character))
                {
                    follower_position_ = c->getPosition();
                }
            }

            if(heroes.size())
            {
                auto const closest = *std::min_element(heroes.begin(), heroes.end(), [this](auto const& h1, auto const& h2)
                {
                    return utility::length(getPosition() - h1->getPosition()) < utility::length(getPosition() - h2->getPosition());
                });

                // Start targetting from the hero's position...
                target_ = to_maze_coordinates(closest->getPosition());

                // ...then guess its position two tiles ahead...
                auto h = closest->heading();
                for(int i = 0; i != 2; ++i)
                {
                    // Get the nature of the tiles around the target.
                    auto around = maze_->around(target_);

                    // Remove the tile that would be opposite of the heading.
                    around.erase(~h);

                    // If there's path straight ahead, pick that...
                    if(maze::to_structure(around.at(h)) == maze::structure::path)
                    {
                        target_ = target_ + to_vector2i(h);
                    }
                    // ...else pick the first other possible path.
                    else
                    {
                        // Remove the choice that's ahead since ahead is not a path.
                        around.erase(h);

                        // If the first choice is not a path, remove it.
                        if(maze::to_structure(around.begin()->second) != maze::structure::path)
                        {
                            around.erase(around.begin());
                        }

                        // We're left with the first choice being a path for sure.
                        h = around.begin()->first;
                        target_ = target_ + to_vector2i(h);
                    }
                }

                // ...then double the vector from follower to target.
                follower_to_ahead_ = to_playground_coordinates(target_) - follower_position_;
                auto const unit = utility::unit(follower_to_ahead_) * (float)level::tile_size;
                auto t = to_playground_coordinates(target_) + follower_to_ahead_;

                // "Backtrack" `t` along `v`'s axis until it is within bounds and is a `path`.
                while(!sf::IntRect{0, 0, level::width, level::height}.contains(to_maze_coordinates(t)) ||
                      maze::to_structure((*maze_)[to_maze_coordinates(t)]) != maze::structure::path)
                {
                    t -= unit;
                }
                ahead_to_target_ = t - to_playground_coordinates(target_);

                target_ = to_maze_coordinates(t);
            }
            
            spdlog::info("{} targeting through axis [{}, {}]", name(), target_.x, target_.y);
        }
    }}));

    enemy::update_self(dt, commands);
}

void axis::draw_self(
    sf::RenderTarget& target,
    sf::RenderStates states) const
{
    // Show the computations done to arrive at the target.
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::T) &&
       current_mode_ == mode::chase)
    {
            sf::RectangleShape a{{utility::length(follower_to_ahead_), 5}};
            a.setPosition(follower_position_);
            a.setRotation(utility::to_degree(std::atan2(follower_to_ahead_.y, follower_to_ahead_.x)));
            a.setFillColor(sf::Color::Cyan * sf::Color{255, 255, 255, 128});

            sf::RectangleShape b{{utility::length(ahead_to_target_), 5}};
            b.setPosition(follower_position_ + follower_to_ahead_);
            b.setRotation(utility::to_degree(std::atan2(ahead_to_target_.y, ahead_to_target_.x)));
            b.setFillColor(sf::Color::Magenta * sf::Color{255, 255, 255, 128});

            target.draw(a, {parent->world_transform()});
            target.draw(b, {parent->world_transform()});
    }

    enemy::draw_self(target, states);
}

}
