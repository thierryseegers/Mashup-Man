#pragma once

#include "scene.h"

namespace layer
{

enum class id
{
    // These are in the order in which they are rendered.
    background,
    maze,
    items,
    characters,
    projectiles,
    pipes,
    animations
};

class background
    : public scene::node
{};

class maze
    : public scene::node
{};

class items
    : public scene::node
{};

class characters
    : public scene::node
{};

class projectiles
    : public scene::node
{};

class pipes
    : public scene::node
{};

class animations
    : public scene::node
{};

}
