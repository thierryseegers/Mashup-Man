#pragma once

// #include "music.h"
#include "player.h"
#include "sound.h"
#include "state/state.h"

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

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
    sf::RenderWindow window;
    player player_1;
    player player_2;
    sound::player sound;

    state::stack states;

    sf::Text statistics_text;
    sf::Time statistics_update_time;
    std::size_t statistics_num_frames;

    sf::Time const time_per_frame = sf::seconds(1.f / 60.f);
};
