#include "resources.h"

#include <entt/resource/resource.hpp>
#include <magic_enum.hpp>
#include <SFML/Audio/SoundBuffer.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Texture.hpp>

#include <stdexcept>
#include <string>
#include <string_view>

namespace resources
{

using namespace std::string_literals;
namespace me = magic_enum;

template<>
std::string_view resource_path<font>(font const id)
{
    switch(id)
    {
        case font::main: return "assets/fonts/crackman.ttf"; break;
        case font::retro: return "assets/fonts/Retro Gaming.ttf"; break;
        default:
            throw std::runtime_error("unknown font id [" + std::string{me::enum_name(id)} + "]");
    }
}

template<>
std::string_view resource_path<texture>(texture const id)
{
    switch(id)
    {
        case texture::background: return "assets/images/NES - Super Mario Bros - Background.png"; break;
        case texture::brothers: return "assets/images/NES - Super Mario Bros - Mario & Luigi.png"; break;
        case texture::enemies: return "assets/images/NES - Super Mario Bros - Enemies & Bosses.png"; break;
        case texture::features: return "assets/images/NES - Super Mario Bros - Tileset.png"; break;
        case texture::items: return "assets/images/NES - Super Mario Bros - Items Objects and NPCs.png"; break;
        case texture::walls: return "assets/images/walls.png"; break;
        default:
            throw std::runtime_error("unknown texture id [" + std::string{magic_enum::enum_name(id)} + "]");
    }
}

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

template<>
auto get<texture>(texture const id) -> entt::resource<sf::Texture>
{
    static entt::resource_cache<sf::Texture, texture_resource_loader> cache;
    switch (id)
    {
        case texture::brothers:
        case texture::enemies:
        case texture::features:
        case texture::items:
            return cache.load(magic_enum::enum_integer(id), texture_resource_loader::with_image_mask_tag{}, resource_path(id)).first->second;
        case texture::pipe:
            return cache.load(magic_enum::enum_integer(id), texture_resource_loader::pipe_tag{}).first->second;;
        default:
            return cache.load(magic_enum::enum_integer(id), resource_path(id)).first->second;
    }
}

template<>
std::string_view resource_path<sound_effect>(sound_effect const id)
{
    switch(id)
    {
        case sound_effect::bump: return "assets/sounds/Bump.wav"; break;
        case sound_effect::collect_coin: return "assets/sounds/Coin.wav"; break;
        case sound_effect::collect_powerup: return "assets/sounds/Powerup.wav"; break;
        case sound_effect::die: return "assets/sounds/Die.wav"; break;
        case sound_effect::fireball: return "assets/sounds/Fire ball.wav"; break;
        case sound_effect::fireworks: return "assets/sounds/Fireworks.wav"; break;
        case sound_effect::game_over: return "assets/sounds/Game Over.wav"; break;
        case sound_effect::kick: return "assets/sounds/Kick.wav"; break;
        case sound_effect::level_complete: return "assets/sounds/Stage Clear.wav"; break;
        case sound_effect::pause: return "assets/sounds/Pause.wav"; break;
        case sound_effect::warp: return "assets/sounds/Warp.wav"; break;
        default:
            throw std::runtime_error("unknown sound_effect id [" + std::string{magic_enum::enum_name(id)} + "]");
    }
}

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

template<>
auto get<sound_effect>(sound_effect const id) -> entt::resource<sf::SoundBuffer>
{
    static entt::resource_cache<sf::SoundBuffer, sound_resource_loader> cache;
    switch (id)
    {
        case sound_effect::short_die:
            return cache.load(magic_enum::enum_integer(id), sound_resource_loader::short_die_tag{}).first->second;;
        default:
            return cache.load(magic_enum::enum_integer(id), resource_path(id)).first->second;
    }
}

}
