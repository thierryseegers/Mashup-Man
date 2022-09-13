#include "application.h"

#include "registries.h"
#include "systems/destroy_tagged.h"
#include "systems/tag_destroy_stopped_sounds.h"
#include "systems/play_sound_effects.h"

#include "entity/hero.h"
#include "resources.h"
#include "state/states.h"
#include "utility.h"

#include <entt/core/hashed_string.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Audio/Listener.hpp>
#include <SFML/System.hpp>

#include <map>
#include <type_traits>
#include <string>

using namespace entt::literals;

application::application()
    : window{sf::VideoMode(/*1024*/sf::VideoMode::getDesktopMode().width, /*768*/sf::VideoMode::getDesktopMode().height), "Mashup-Man", sf::Style::Default}
    , states{{/*music, */{}, window}}
    , statistics_num_frames{0}
{
    window.setKeyRepeatEnabled(false);
    window.setFramerateLimit(60);


    // auto& shaders = utility::single::mutable_instance<resources::shaders_t>();
    // shaders.load(resources::shader_pass::brightness, "Media/Shaders/Fullpass.vert", "Media/Shaders/Brightness.frag");
    // shaders.load(resources::shader_pass::downsample, "Media/Shaders/Fullpass.vert", "Media/Shaders/DownSample.frag");
    // shaders.load(resources::shader_pass::gaussian_blur, "Media/Shaders/Fullpass.vert", "Media/Shaders/GuassianBlur.frag");
    // shaders.load(resources::shader_pass::add, "Media/Shaders/Fullpass.vert", "Media/Shaders/Add.frag");

    statistics_text.setFont(resources::get(resources::font::main));
    statistics_text.setPosition(5.f, 5.f);
    statistics_text.setCharacterSize(24);

	// states.register_state<state::menu>(state::id::menu);
    states.register_state<state::character_select>(state::id::character_select);
    states.register_state<state::game_over>(state::id::game_over);
    states.register_state<state::game>(state::id::game);
    states.register_state<state::level_complete>(state::id::level_complete);
    states.register_state<state::pause>(state::id::pause);
    // states.register_state<state::settings>(state::id::settings);
    states.register_state<state::title>(state::id::title);

    states.request_push(state::id::title);

    // music.volume(25.f);
    services::initialize();
    registries::audio.ctx().emplace_hint<sf::Vector3f>("listener_position"_hs, sf::Listener::getPosition());
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
            if(states.empty())
            {
                window.close();
            }
        }

        update_statistics(elapsed_time);
        render();

        systems::play_sound_effects();
        systems::tag_destroy_stopped_sounds();
        systems::destroy_tagged();
    }
}

void application::process_input()
{
    sf::Event event;
    while(window.pollEvent(event))
    {
        if(event.type == sf::Event::Resized)
        {
            window.setView(sf::View{sf::FloatRect{0, 0, (float)event.size.width, (float)event.size.height}});
        }

        states.handle_event(event);

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
    sf::Time const& dt)
{
    states.update(dt);
}

void application::render()
{
    window.clear();
    states.draw();

    // window.draw(statistics_text);
    window.display();
}