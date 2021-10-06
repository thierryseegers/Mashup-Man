#pragma once

#include "command.h"
// #include "effects/bloom.h"
#include "entity/entities.h"
#include "layer.h"
#include "level.h"
#include "maze.h"
#include "lifeboard.h"
#include "resources.h"
#include "scene.h"
#include "scoreboard.h"
#include "state/stack.h"
#include "sound.h"

#include <magic_enum.hpp>
#include <SFML/Graphics.hpp>

#include <functional>
#include <memory>

class world
{
public:
    explicit world(
        state::stack::context_t& context);

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

    void update_enemies(
        sf::Time const dt);

    sf::RenderTarget& target;

    // effect::bloom bloom_effect;
    sound::player& sound;

    scene::node playground;
    scene::layers<magic_enum::enum_count<layer::id>()> layers;
    maze *maze_;
    scoreboard scoreboard_;
    lifeboard lifeboard_;

    commands_t commands_;

    // Heroes infos.
    struct hero
    {
        entity::hero *hero_;
        int lives;

        sf::Vector2f spawn_point;
        sf::Time spawn_timer;

        std::function<std::unique_ptr<entity::hero> ()> maker;
    };
    std::vector<hero> heroes;

    // Number of pills to eat on this stage.
    int n_pills;

    level::grid<entity::entity*> immovables;

    entity::enemy::mode enemy_mode_;
    sf::Time enemy_mode_timer; // Time left in current mode.

    sf::Time done_timer; // Time to show an empty world after all heroes are dead.
};
