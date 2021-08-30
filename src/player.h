#pragma once

#include "action.h"
#include "command.h"

#include <SFML/Window.hpp>

#include <map>

class player_t
{
public:
    // enum class mission
    // {
    //     failure,
    //     running,
    //     success,
    // };

    player_t();

    void handle_event(
        sf::Event const& event,
        commands_t& commands);

    void handle_realtime_input(
        commands_t& commands);

    // mission& mission_status()
    // {
    //     return status;
    // }

    // [[nodiscard]] mission mission_status() const
    // {
    //     return status;
    // }

private:
    [[nodiscard]] static bool is_realtime_action(action const a);

    std::map<action, command_t> action_bindings;

    // mission status;
};