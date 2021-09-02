#pragma once

#include "command.h"
// #include "effects/bloom.h"
// #include "entity/enemy.h"
#include "entity/brother.h"
#include "level.h"
#include "resources.h"
#include "scene.h"
#include "sound.h"

#include <magic_enum.hpp>
#include <SFML/Graphics.hpp>

#include <array>
#include <set>
#include <vector>

namespace layer
{

enum class id
{
    maze,
    items,
    characters,
    pipes
};

class maze :
    public scene::node
{};

class items :
    public scene::node
{};

class characters :
    public scene::node
{};

class pipes :
    public scene::node
{};
}

class world_t
{
public:
    explicit world_t(
        sf::RenderTarget& output_target,
        sound::player& sound);

    void update(
        sf::Time const dt);
    void draw();

    commands_t& commands();

    // [[nodiscard]] bool player_alive() const;

    // [[nodiscard]] bool player_reached_end() const;

private:
    void build_scene();
    
    // void remove_unviewables();
    void handle_collisions();

    // sf::FloatRect view_bounds() const;
    // sf::FloatRect battlefield_bounds() const;

    sf::RenderTarget& target;
    sf::View view;

    // effect::bloom bloom_effect;
    sound::player& sound;

    scene::node graph;
    scene::layers<magic_enum::enum_count<layer::id>()> layers;

    commands_t commands_;

    entity::brother* mario;

    level::info level_info;
    level::grid<entity::entity*> immovables;
    std::vector<entity::entity*> characters;
};