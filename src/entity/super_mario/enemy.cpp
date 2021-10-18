#include "entity/super_mario/enemy.h"

#include "configuration.h"
#include "entity/enemy.h"
#include "resources.h"

#include <magic_enum.hpp>
#include <SFML/Graphics.hpp>

#include <array>
#include <string_view>
#include <vector>

namespace entity::super_mario
{

namespace me = magic_enum;

using mode = ::entity::enemy::mode;

using animated_sprite_rects =
    std::array<
        std::vector<sf::IntRect>,
        me::enum_count<mode>()>;

std::vector<sf::IntRect> goomba_animated_sprite_rects(
    mode const mode_)
{
    static animated_sprite_rects const rects = []{
        animated_sprite_rects r;

        r[me::enum_integer(mode::confined)] = std::vector<sf::IntRect>{{1, 28, 16, 16}, {18, 28, 16, 16}};
        r[me::enum_integer(mode::chase)] = r[me::enum_integer(mode::confined)];
        r[me::enum_integer(mode::frightened)] = std::vector<sf::IntRect>{{1, 166, 16, 16}, {18, 166, 16, 16}};
        r[me::enum_integer(mode::scatter)] = r[me::enum_integer(mode::confined)];

        return r;
    }();

    return rects[me::enum_integer(mode_)];
}

sf::IntRect goomba_dead_sprite_rect()
{
    return sf::IntRect{39, 28, 16, 16};
}

goomba::goomba()
    : follower{
        sprite{
            resources::texture::enemies,
            goomba_animated_sprite_rects(mode::scatter),
            sf::seconds(0.25f),
            sprite::repeat::loop,
            configuration::values()["enemies"]["goomba"]["scale"].value_or<float>(1.f),
        },
        *configuration::values()["enemies"]["goomba"]["speed"].value<int>(),
        direction::left
        }
{}

std::string_view goomba::name() const
{
    return "goomba";
}

void goomba::update_sprite()
{
    if(current_mode_ == mode::dead)
    {
        sprite_.still(goomba_dead_sprite_rect());
    }
    else
    {
        sprite_.animate(goomba_animated_sprite_rects(current_mode_), sf::seconds(0.25f), sprite::repeat::loop);
    }

    enemy::update_sprite();
}

std::vector<sf::IntRect> koopa_animated_sprite_rects(
    mode const mode_)
{
    static animated_sprite_rects const rects = []{
        animated_sprite_rects r;

        // Koopas are a bit taller than 16 pixels but don't take the entire 32 pixels of height either.
        r[me::enum_integer(mode::confined)] = std::vector<sf::IntRect>{{76, 21, -16, 23}, {93, 21, -16, 23}};
        r[me::enum_integer(mode::chase)] = r[me::enum_integer(mode::confined)];
        r[me::enum_integer(mode::frightened)] = std::vector<sf::IntRect>{{76, 159, -16, 23}, {93, 159, -16, 23}};
        r[me::enum_integer(mode::scatter)] = r[me::enum_integer(mode::confined)];

        return r;
    }();

    return rects[me::enum_integer(mode_)];
}

sf::IntRect koopa_dead_sprite_rect()
{
    return sf::IntRect{136, 28, 16, 16};
}

koopa::koopa()
    : ahead{
        sprite{
            resources::texture::enemies,
            goomba_animated_sprite_rects(mode::scatter),
            sf::seconds(0.25f),
            sprite::repeat::loop,
            configuration::values()["enemies"]["koopa"]["scale"].value_or<float>(1.f),
        },
        *configuration::values()["enemies"]["koopa"]["speed"].value<int>(),
        direction::left
        }
{}

std::string_view koopa::name() const
{
    return "koopa";
}

void koopa::update_sprite()
{
    if(current_mode_ == mode::dead)
    {
        sprite_.still(koopa_dead_sprite_rect());
    }
    else
    {
        sprite_.animate(koopa_animated_sprite_rects(current_mode_), sf::seconds(0.25f), sprite::repeat::loop);
    }

    enemy::update_sprite();
}

}
