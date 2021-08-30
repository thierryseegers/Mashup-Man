#pragma once

#include "command.h"
// #include "effects/bloom.h"
// #include "entity/enemy.h"
// #include "entity/leader.h"
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
    characters
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
    // void load_textures();
    void build_scene();
    
    // void remove_unviewables();
    void handle_collisions();
    // void guide_missiles();
    // void spawn_enemies();

    // sf::FloatRect view_bounds() const;
    // sf::FloatRect battlefield_bounds() const;

    sf::RenderTarget& target;
    // sf::RenderTexture scene_texture;
    sf::View view;

    // effect::bloom bloom_effect;
    sound::player& sound;

    scene::node graph;
    scene::layers<magic_enum::enum_count<layer::id>()> layers;

    commands_t commands_;

    // sf::FloatRect const bounds;
    // sf::Vector2f const player_spawn_point;
    // float scroll_speed;
    
    // entity::brother* player;
    // std::vector<entity::enemy*> enemies;

    // struct spawn
    // {
    //     std::function<std::unique_ptr<entity::character> ()> const what;
    //     sf::Vector2f const where;

    //     bool operator<(spawn const& other) const
    //     {
    //         return where.y < other.where.y;
    //     }
    // };

    // std::multiset<spawn> enemy_spawns;

    level::info level_info;
};