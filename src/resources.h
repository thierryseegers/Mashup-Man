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

// Default resource_loader which simply load from file with `loadFromFile`.
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

// Main fucntion to retrieve resources.
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

template<>
std::string_view resource_path<font>(font const);

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

template<>
std::string_view resource_path<texture>(texture const);

// Some textures must be created in a special fashion. Therefore we use a loader 
// with overloaded `operator()` for the special cases.
struct texture_resource_loader : resource_loader<sf::Texture>
{
    using resource_loader<sf::Texture>::operator();

    struct with_image_mask_tag{};
    struct pipe_tag{};

    result_type operator()(
        with_image_mask_tag const,
        std::string_view const path)
    {
        sf::Image image;
        if(!image.loadFromFile(std::string{path}))
        {
            throw std::runtime_error("resource_loader failed to load [" + std::string{path} + "]");
        }
        image.createMaskFromColor({147, 187, 236});

        auto resource = std::make_shared<sf::Texture>();
        resource->loadFromImage(image);
        return resource;
    }

    result_type operator()(
        pipe_tag const)
    {
        // Craft the pipe texture from its four tiles in items tileset.
        sf::Image tileset;
        if(!tileset.loadFromFile("assets/images/NES - Super Mario Bros - Tileset.png"))
        {
            throw std::runtime_error("resource_loader failed to load");
        }

        auto resource = std::make_shared<sf::Texture>();
        resource->create(32, 32);
        for(auto const y : std::initializer_list<int>{0, 1})
        {
            for(auto const x : std::initializer_list<int>{0, 1})
            {
                sf::Image corner;
                corner.create(16, 16);
                corner.copy(tileset, 0, 0, sf::IntRect{69 + (x * 17), 80 + (y * 17), 16, 16});
                corner.createMaskFromColor({147, 187, 236});
                resource->update(corner, x * 16, y * 16);
            }
        }
        return resource;
    }
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

template<>
std::string_view resource_path<sound_effect>(sound_effect const);

template<>
auto get<sound_effect>(sound_effect const id) -> entt::resource<sf::SoundBuffer>;

// Some sound buffers must be created in a special fashion. Therefore we use a loader 
// with overloaded `operator()` for the special cases.
struct sound_resource_loader : resource_loader<sf::SoundBuffer>
{
    using resource_loader<sf::SoundBuffer>::operator();

    struct short_die_tag{};

    result_type operator()(
        short_die_tag)
    {
        // The `short_die` effect is created from the beginning portion of the `die` effect.
        auto const die_buffer = resources::get(resources::sound_effect::die);
        auto const *const die_samples = die_buffer->getSamples();
        auto resource = std::make_shared<sf::SoundBuffer>();
        resource->loadFromSamples(die_samples, die_buffer->getSampleRate() * 0.6, die_buffer->getChannelCount(), die_buffer->getSampleRate());
        return resource;
    }
};

// Overloaded `get` which uses `sound_resource_loader`.
template<>
auto get<sound_effect>(sound_effect const id) -> entt::resource<sf::SoundBuffer>;

}
