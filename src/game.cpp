#include "game.h"

#include "resources.h"
// #include "state/game_over.h"
// #include "state/game.h"
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

game::game()
    : window{sf::VideoMode(1024, 768), "SuperPacBros", sf::Style::Close}
    // , states{{music, player_1, sound, window}}
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
	// states.register_state<state::game>(state::id::game);
	// states.register_state<state::pause>(state::id::pause);
    // states.register_state<state::settings>(state::id::settings);
    // states.register_state<state::game_over>(state::id::game_over);

    // states.request_push(state::id::title);

    // music.volume(25.f);

    // Load the maze's tile strip.
    maze.load(textures.get(resources::texture::maze_tiles));

    // The maze's pixels.
    // Extract the maze's wall information from the map.
    std::ifstream map{"assets/maps/1.txt"};
    std::string line;
    std::array<std::array<int, 28>, 33> maze_tile_values;
    size_t row = 0, column = 0;
    while(std::getline(map, line))
    {
        std::for_each(line.begin(), line.end(), [&](char const c)
        {
            switch(c)
            {
                case '0':
                case '1':
                case '2':
                    maze_tile_values[row][column] = c - '0';
                    break;
                default:
                    maze_tile_values[row][column] = 4;
                    break;
            }
            ++column;
        });
        ++row;
        column = 0;
    }
    maze.layout(maze_tile_values);
}

void game::run()
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

void game::process_input()
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

void game::update_statistics(
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

void game::update(
    sf::Time const& /*dt*/)
{
    // states.update(dt);
}

void game::render()
{
    window.clear();
    // states.draw();

    window.setView(window.getDefaultView());
    // sf::Sprite maze_sp{maze_tx};
    // maze_sp.setOrigin(0, 0);
    // maze_sp.set
    // window.draw(sf::Sprite{maze_tx});
    window.draw(maze);
    window.draw(statistics_text);
    window.display();
}