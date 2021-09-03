#pragma once

#include "command.h"
// #include "effects/bloom.h"
#include "entity/entities.h"
#include "layer.h"
#include "level.h"
#include "resources.h"
#include "scene.h"
#include "sound.h"

#include <magic_enum.hpp>
#include <SFML/Graphics.hpp>

#include <list>

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
    
    void handle_collisions();

    void update_brother(
        entity::brother *bro);

    void update_fireballs();

    void update_enemies();

    // sf::FloatRect view_bounds() const;
    // sf::FloatRect battlefield_bounds() const;

    sf::RenderTarget& target;
    sf::View view;

    // effect::bloom bloom_effect;
    sound::player& sound;

    scene::node graph;
    scene::layers<magic_enum::enum_count<layer::id>()> layers;

    commands_t commands_;

    entity::brother *mario, *luigi;

    level::info level_info;

    level::grid<entity::entity*> immovables;
};