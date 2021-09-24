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
    enum class outcome
    {
        failure,
        success,
    };

    using hero_maker_f = std::function<std::unique_ptr<entity::hero>()>;

    player(
        unsigned int const id,
        hero_maker_f const hero_maker_);

    hero_maker_f hero_maker() const;

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

    // 
    unsigned int id;
    hero_maker_f hero_maker_;

    outcome outcome_;
};
