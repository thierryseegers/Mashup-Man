#pragma once

#include "scene.h"

namespace layer
{

enum class id
{
    // These are in the order in which they are rendered.
    maze,
    items,
    characters,
    projectiles,
    pipes
};

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

}
