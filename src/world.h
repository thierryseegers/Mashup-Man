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

class world
{
public:
    explicit world(
        sf::RenderTarget& output_target,
        sound::player& sound);

    void update(
        sf::Time const dt);

    void draw();

    commands_t& commands();

    [[nodiscard]] bool players_alive() const;

    [[nodiscard]] bool players_done() const;

    void handle_size_changed(
        sf::Event::SizeEvent const& event);

private:
    void build_scene();

    void handle_collisions();

    void update_brother(
        entity::brother *bro);

    void update_fireballs();

    void update_enemies(
        sf::Time const dt);

    // sf::FloatRect view_bounds() const;
    // sf::FloatRect battlefield_bounds() const;

    sf::RenderTarget& target;
    sf::View view;

    // effect::bloom bloom_effect;
    sound::player& sound;

    scene::node graph;
    scene::layers<magic_enum::enum_count<layer::id>()> layers;

    commands_t commands_;

    // Brothers infos.
    entity::brother *mario, *luigi;
    float mario_spawn_x, mario_spawn_y, luigi_spawn_x, luigi_spawn_y;
    sf::Time mario_spawn_timer, luigi_spawn_timer;
    int mario_lives, luigi_lives;

    // Number of pills to eat on this stage.
    int n_pills;

    level::info level_info;
    level::grid<entity::entity*> immovables;

    entity::enemy::mode enemy_mode_;
    sf::Time enemy_mode_timer; // Time left in current mode.
};