#include "bindings.h"

#include "tomlpp.h"

#include <magic_enum.hpp>

#include <fstream>
#include <filesystem>
#include <string_view>

namespace bindings
{

toml::table read()
{
    using namespace std::string_view_literals;

    if(std::filesystem::exists("./bindings.toml"))
    {
        return toml::parse_file("./bindings.toml");
    }
    else
    {
        static constexpr std::string_view default_bindings = R"(
            [keyboard]
            go_up = 73
            go_down = 74
            go_left = 71
            go_right = 72
            fire = 57

            [joystick]
            fire = 0
        )"sv;

        return toml::parse(default_bindings);
    }
}

keyboard_bindings_t& keyboard()
{
    static keyboard_bindings_t keyboard_bindings = []
    {
        keyboard_bindings_t kb;

        auto const& bindings = read();
        for(auto const& binding : *bindings["keyboard"].as_table())
        {
            kb[static_cast<sf::Keyboard::Key>(*binding.second.value<int64_t>())] = magic_enum::enum_cast<action>(binding.first).value();
        }

        return kb;
    }();

    return keyboard_bindings;
}

joystick_bindings_t& joystick()
{
    static joystick_bindings_t joystick_bindings = []
    {
        joystick_bindings_t jb;

        auto const& bindings = read();
        for(auto const& binding : *bindings["joystick"].as_table())
        {
            jb[*binding.second.value<unsigned int>()] = magic_enum::enum_cast<action>(binding.first).value();
        }

        return jb;
    }();

    return joystick_bindings;
}

void save()
{
    toml::table bindings;
    bindings.insert("keyboard", toml::table{});
    for(auto const& binding : bindings::keyboard())
    {
        bindings["keyboard"].as_table()->insert(magic_enum::enum_name<action>(binding.second), static_cast<int64_t>(binding.first));
    }

    bindings.insert("joystick", toml::table{});
    for(auto const& binding : bindings::joystick())
    {
        bindings["joystick"].as_table()->insert(magic_enum::enum_name<action>(binding.second), static_cast<int64_t>(binding.first));
    }

    std::ofstream bindings_file("./bindings.toml");
    bindings_file << bindings << std::endl;
}

void bind_key(
    sf::Keyboard::Key const key,
    action const what)
{
    keyboard().erase(bindings::bound_key(what));
    keyboard()[key] = what;
}

void bind_button(
    unsigned int const button,
    action const what)
{
    joystick().erase(bindings::bound_button(what));
    joystick()[button] = what;
}


sf::Keyboard::Key bound_key(
    action const what)
{
    for(auto const& [key, a] : bindings::keyboard())
    {
        if(a == what)
        {
            return key;
        }
    }

    return sf::Keyboard::Unknown;
}

unsigned int bound_button(
    action const what)
{
    for(auto const& [button, a] : bindings::joystick())
    {
        if(a == what)
        {
            return button;
        }
    }

    return -1;
}

}
