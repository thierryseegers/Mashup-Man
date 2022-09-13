#pragma once

#include <entt/resource/cache.hpp>
#include <entt/resource/resource.hpp>
#include <magic_enum.hpp>
#include <SFML/Audio/SoundBuffer.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Texture.hpp>

#include <memory>
#include <stdexcept>
#include <string>
#include <string_view>

namespace resources
{

namespace me = magic_enum;

// Default resource_loader which simply loads from file with `loadFromFile`.
template<class Resource>
struct resource_loader {
    using result_type = std::shared_ptr<Resource>;

    result_type operator()(
        std::string_view path)
    {
        auto resource = std::make_shared<Resource>();
        if(!resource->loadFromFile(std::string{path}))
        {
            throw std::runtime_error("resource_loader failed to load [" + std::string{path} + "]");
        }
        return resource;
    }
};

// Look-up function for the path of resource given their enum value.
template<class Id>
std::string_view resource_path(Id const id);

template<class Id>
struct resource_type;

template<class Id>
using resource_type_t = typename resource_type<Id>::type;

// Main function to retrieve resources.
// Because its return type is dependent on the resource type, we map `Id` to its 
// corresponding SFML type.
template<class Id, class Resource = resource_type_t<Id>>
entt::resource<Resource> get(Id const id)
{
    static entt::resource_cache<Resource, resource_loader<Resource>> cache;
    return cache.load(me::enum_integer(id), resource_path(id)).first->second;
}

enum class font : entt::id_type
{
    main,
    retro,
};

template<>
struct resource_type<font>
{
    using type = sf::Font;
};

enum class texture : entt::id_type
{
    background,
    brothers,
    enemies,
    features,
    items,
    pipe,
    walls,
    // buttons,
    // title_screen,
};

template<>
struct resource_type<texture>
{
    using type = sf::Texture;
};

// Overloaded `get` which uses `texture_resource_loader`.
template<>
auto get<texture>(texture const id) -> entt::resource<sf::Texture>;

enum class sound_effect
{
    bump,
    collect_coin,
    collect_powerup,
    die,
    fireball,
    fireworks,
    game_over,
    kick,
    level_complete,
    pause,
    short_die,
    warp
    // button
};

template<>
struct resource_type<sound_effect>
{
    using type = sf::SoundBuffer;
};

// Overloaded `get` which uses `sound_resource_loader`.
template<>
auto get<sound_effect>(sound_effect const id) -> entt::resource<sf::SoundBuffer>;

}
