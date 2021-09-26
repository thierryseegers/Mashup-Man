#pragma once

#include <SFML/System.hpp>

#include <functional>
#include <queue>

namespace scene
{
    class node;
}

using command_t = std::function<void (scene::node&, sf::Time const&)>;

using commands_t = std::queue<command_t>;

template<typename Object>
command_t make_command(
    std::function<void (Object&, sf::Time const&)> f)
{
    return [=](scene::node& n, sf::Time const& dt)
    {
        if(Object *p = dynamic_cast<Object*>(&n))
        {
            f(*p, dt);
        }
    };
}

template<typename Object>
command_t make_command(
    void (*f)(Object&, sf::Time const&))
{
    return make_command<Object>(std::function{f});
}
