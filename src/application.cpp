#include "application.h"

#include "resources.h"
// #include "state/game_over.h"
#include "state/game.h"
// #include "state/id.h"
// #include "state/menu.h"
// #include "state/pause.h"
// #include "state/settings.h"
// #include "state/title.h"
#include "utility.h"

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include <filesystem>
#include <memory>
#include <stdexcept>

application::application()
    : window{sf::VideoMode(1024, 768), "SuperPacBros", sf::Style::Close}
    , states{{/*music, player_1,*/ sound, window}}
    , statistics_num_frames{0}
{
    window.setKeyRepeatEnabled(false);
    window.setFramerateLimit(60);

    auto& fonts = utility::single::mutable_instance<resources::fonts_t>();
    fonts.load(resources::font::main, "assets/fonts/Sansation.ttf");
    fonts.load(resources::font::label, "assets/fonts/Sansation.ttf");

    auto& textures = utility::single::mutable_instance<resources::textures_t>();
    textures.load(resources::texture::maze_tiles, "assets/images/maze-tiles.png");
    // textures.load(resources::texture::title_screen, "Media/Textures/TitleScreen.png");
	// textures.load(resources::texture::buttons, "Media/Textures/Buttons.png");

    // auto& shaders = utility::single::mutable_instance<resources::shaders_t>();
    // shaders.load(resources::shader_pass::brightness, "Media/Shaders/Fullpass.vert", "Media/Shaders/Brightness.frag");
    // shaders.load(resources::shader_pass::downsample, "Media/Shaders/Fullpass.vert", "Media/Shaders/DownSample.frag");
    // shaders.load(resources::shader_pass::gaussian_blur, "Media/Shaders/Fullpass.vert", "Media/Shaders/GuassianBlur.frag");
    // shaders.load(resources::shader_pass::add, "Media/Shaders/Fullpass.vert", "Media/Shaders/Add.frag");

    statistics_text.setFont(fonts.get(resources::font::main));
    statistics_text.setPosition(5.f, 5.f);
    statistics_text.setCharacterSize(24);

    // states.register_state<state::title>(state::id::title);
	// states.register_state<state::menu>(state::id::menu);
	states.register_state<state::game>(state::id::game);
	// states.register_state<state::pause>(state::id::pause);
    // states.register_state<state::settings>(state::id::settings);
    // states.register_state<state::game_over>(state::id::game_over);

    states.request_push(state::id::game);

    // music.volume(25.f);

    // Load the walls's tile strip.
    walls.load(textures.get(resources::texture::maze_tiles));


    // Read in the maze tile information.
    std::ifstream map{"assets/maps/1.txt"};
    size_t row = 0;
    for(std::string line; std::getline(map, line);)
    {
        std::copy(line.begin(), line.end(), maze[row++].begin());
    }

    // Read in wall tile rotation information.
    std::array<std::array<int, 28>, 33> wall_tile_rotations;
    std::string line;
    for(auto& row : wall_tile_rotations)
    {
        size_t column = 0;
        std::getline(map, line);
        std::for_each(line.begin(), line.end(), [&](char const c)
        {
            switch(c)
            {
                case '0':
                case '1':
                case '2':
                case '3':
                    row[column] = c - '0';
                    break;
                default:
                    row[column] = 0;
                    break;
            }
            ++column;
        });
        column = 0;
    }

    // Extract just the wall tile information.
    std::array<std::array<int, 28>, 33> wall_tiles;
    for(size_t r = 0; r != 33; ++r)
    {
        for(size_t c = 0; c != 28; ++c)
        {
            switch(maze[r][c])
            {
                case '0':
                case '1':
                case '2':
                case '3':
                case '4':
                    wall_tiles[r][c] = c - '0';
                    break;
                default:
                    wall_tiles[r][c] = 5;
                    break;
            }
        }
    }

    walls.layout(wall_tiles, wall_tile_rotations);
}

void application::run()
{
    sf::Clock clock;
    sf::Time last_update = sf::Time::Zero;

    while(window.isOpen())
    {
        sf::Time const elapsed_time = clock.restart();
        last_update += elapsed_time;
        while(last_update > time_per_frame)
        {
            last_update -= time_per_frame;
            process_input();
            update(time_per_frame);

            // Check inside this loop, because stack might be empty before update() call
            // if(states.empty())
            // {
            //     window.close();
            // }
        }

        update_statistics(elapsed_time);
        render();
    }
}

void application::process_input()
{
    sf::Event event;
    while(window.pollEvent(event))
    {
        // states.handle_event(event);

        if(event.type == sf::Event::Closed)
        {
            window.close();
        }
    }
}

void application::update_statistics(
    sf::Time const& dt)
{
    statistics_update_time += dt;
    ++statistics_num_frames;

    if(statistics_update_time >= sf::seconds(1.0f))
    {
        statistics_text.setString(
            "Frames / Second = " + std::to_string(statistics_num_frames) + "\n" +
            "Time / Update = " + std::to_string(statistics_update_time.asMicroseconds() / statistics_num_frames) + "us");

        statistics_update_time -= sf::seconds(1.0f);
        statistics_num_frames = 0;
    }
}

void application::update(
    sf::Time const& /*dt*/)
{
    // states.update(dt);
}

void application::render()
{
    window.clear();
    // states.draw();

    window.setView(window.getDefaultView());

    window.draw(walls);
    window.draw(statistics_text);
    window.display();
}