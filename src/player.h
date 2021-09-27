#pragma once

#include "action.h"
#include "command.h"
#include "direction.h"
#include "entity/hero.h"

#include <SFML/Window.hpp>

#include <functional>
#include <map>
#include <memory>
#include <string>
#include <type_traits>

class player
{
public:
    using hero_maker_f = std::function<std::unique_ptr<entity::hero>()>;

    template<typename Hero>
    player(
        std::type_identity<Hero>)
        : id{Hero::id()}
        , hero_maker_{[]{ return std::make_unique<Hero>(); }}
    {
        action_bindings[action::cruise] = make_command(+[](Hero& hero, sf::Time const&)
            {
                hero.steer(direction::none);
            });
        action_bindings[action::head_down] = make_command(+[](Hero& hero, sf::Time const&)
            {
                hero.steer(direction::down);
            });
        action_bindings[action::head_left] = make_command(+[](Hero& hero, sf::Time const&)
            {
                hero.steer(direction::left);
            });
        action_bindings[action::head_right] = make_command(+[](Hero& hero, sf::Time const&)
            {
                hero.steer(direction::right);
            });
        action_bindings[action::head_up] = make_command(+[](Hero& hero, sf::Time const&)
            {
                hero.steer(direction::up);
            });

        action_bindings[action::attack] = make_command(+[](Hero& hero, sf::Time const&)
            {
                hero.attack();
            });
    }

    hero_maker_f hero_maker() const;

    void handle_event(
        sf::Event const& event,
        commands_t& commands);

    void handle_realtime_input(
        commands_t& commands);

private:
    [[nodiscard]] static bool is_realtime_action(action const a);

    std::map<action, command_t> action_bindings;

    unsigned int id;
    hero_maker_f hero_maker_;
};
