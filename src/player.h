#pragma once

#include "action.h"
#include "command.h"
#include "direction.h"
#include "entity/brother.h"

#include <SFML/Window.hpp>

#include <map>
#include <string>
#include <type_traits>

class player
{
public:
    enum class outcome
    {
        failure,
        success,
    };

    template<typename Brother>
    player(std::type_identity<Brother>)
        : name{std::is_same_v<Brother, entity::mario> ? "mario" : "luigi"}
        , joystick_id{std::is_same_v<Brother, entity::mario> ? 0 : 1}
        , outcome_{outcome::failure}
    {
        action_bindings[action::cruise] = make_command<Brother>([=](Brother& bro, sf::Time const&)
            {
                bro.steer(direction::none);
            });
        action_bindings[action::head_down] = make_command<Brother>([=](Brother& bro, sf::Time const&)
            {
                bro.steer(direction::down);
            });
        action_bindings[action::head_left] = make_command<Brother>([=](Brother& bro, sf::Time const&)
            {
                bro.steer(direction::left);
            });
        action_bindings[action::head_right] = make_command<Brother>([=](Brother& bro, sf::Time const&)
            {
                bro.steer(direction::right);
            });
        action_bindings[action::head_up] = make_command<Brother>([=](Brother& bro, sf::Time const&)
            {
                bro.steer(direction::up);
            });

        action_bindings[action::fire] = make_command<Brother>([=](Brother& bro, sf::Time const&)
            {
                bro.fire();
            });
    }

    void handle_event(
        sf::Event const& event,
        commands_t& commands);

    void handle_realtime_input(
        commands_t& commands);

    outcome& level_outcome();

    [[nodiscard]] outcome level_outcome() const;

private:
    [[nodiscard]] static bool is_realtime_action(action const a);

    std::map<action, command_t> action_bindings;

    std::string name;
    unsigned int joystick_id;

    outcome outcome_;
};
