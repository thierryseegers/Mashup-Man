#pragma once

#include <SFML/Audio/SoundBuffer.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Shader.hpp>

#include <cassert>
#include <filesystem>
#include <map>
#include <memory>
#include <stdexcept>

namespace resources
{

enum class font
{
    main,
    retro,
};

enum class texture
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

// enum class shader_pass
// {
//     brightness,
//     downsample,
//     gaussian_blur,
//     add,
// };

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

template<typename Type, class Resource>
class holder
{
public:
    void load(
        Type const id,
        std::filesystem::path const& path)
    {
        auto resource = std::make_unique<Resource>();
        if(!resource->loadFromFile(path.string()))
        {
            throw std::runtime_error("resources::holder::load failed to load " + path.string());
        }

        resources[id] = std::move(resource);
    }

    template<typename Parameter>
    void load(
        Type const id,
        std::filesystem::path const& path, Parameter const& parameter)
    {
        auto resource = std::make_unique<Resource>();
        if(!resource->loadFromFile(path.string(), parameter))
        {
            throw std::runtime_error("resources::holder::load failed to load " + path.string());
        }

        resources[id] = std::move(resource);
    }

    void copy(
        Type const id,
        Resource copy)
    {
        auto resource = std::make_unique<Resource>(copy);
        resources[id] = std::move(resource);
    }

    Resource& get(
        Type const id)
    {
        auto const i = resources.find(id);
        assert(i != resources.end());

        return *i->second;
    }

    Resource const& get(
        Type const id) const
    {
        auto const i = resources.find(id);
        assert(i != resources.end());

        return *i->second;
    }

private:
    std::map<Type, std::unique_ptr<Resource>> resources;
};

using fonts_t = holder<font, sf::Font>;
// using shaders_t = holder<shader_pass, sf::Shader>;
using sound_buffers_t = holder<sound_effect, sf::SoundBuffer>;
using textures_t = holder<texture, sf::Texture>;

fonts_t const& fonts();
// shaders_t& shaders();
sound_buffers_t const& sound_buffers();
textures_t const& textures();

}
