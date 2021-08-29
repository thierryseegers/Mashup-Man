#pragma once

// #include "music.h"
// #include "player.h"
#include "sound.h"
#include "state/state.h"

#include "tile_map.h"

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include <array>

class application
{
public:
    application();

    void run();

private:
    void process_input();

    void update_statistics(
        sf::Time const& dt);

    void update(
        sf::Time const& dt);

    void render();

    // music::player music;
    // player_t player_1;
    sound::player sound;
    sf::RenderWindow window;

    state::stack states;

    sf::Text statistics_text;
    sf::Time statistics_update_time;
    std::size_t statistics_num_frames;

    sf::Time const time_per_frame = sf::seconds(1.f / 60.f);

    std::array<std::array<char, 28>, 33> maze;
    tile_map walls;
};