#pragma once

#include "command.h"
// #include "effects/bloom.h"
#include "entity/entities.h"
#include "layer.h"
#include "level.h"
#include "lifeboard.h"
#include "resources.h"
#include "scene.h"
#include "scoreboard.h"
#include "sound.h"

#include <magic_enum.hpp>
#include <SFML/Graphics.hpp>

class world
{
public:
    explicit world(
        sf::RenderTarget& output_target,
        sound::player& sound,
        int const num_players);

    void update(
        sf::Time const dt);

    void draw();

    commands_t& commands();

    [[nodiscard]] bool players_alive() const;

    [[nodiscard]] bool players_done() const;

    void handle_size_changed(
        sf::Event::SizeEvent const& event);

private:
    void build_scene(
        int const num_players);

    void handle_collisions();

    void update_brother(
        entity::brother *bro);

    void update_fireballs();

    void update_enemies(
        sf::Time const dt);

    sf::RenderTarget& target;

    // effect::bloom bloom_effect;
    sound::player& sound;

    scene::node playground;
    scene::layers<magic_enum::enum_count<layer::id>()> layers;
    scoreboard scoreboard_;
    lifeboard lifeboard_;

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

    sf::Time done_timer; // Time to show an empty world after both brothers are dead.
};