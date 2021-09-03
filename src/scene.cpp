#include "scene.h"

#include "utility.h"

#include <SFML/Graphics.hpp>

#include <algorithm>
#include <cassert>
#include <functional>
#include <iterator>
#include <memory>
#include <numeric>
#include <vector>

namespace scene
{

node::iterator::iterator(
    node* root)
{
    if(root)
    {
        indices.push_back(0);
        current = next(root);
    }
    else
    {
        current = nullptr;
    }
}

node::iterator::reference node::iterator::operator*()
{
    return *current;
}

node::iterator::pointer node::iterator::operator->()
{
    return current;
}

node::iterator node::iterator::operator++()
{
    if(current->parent && ++indices.back() != current->parent->children_.size())
    {
        current = next(current->parent->children_[indices.back()].get());
    }
    else
    {
        current = current->parent;
        if(current)
        {
            indices.pop_back();
        }
    }

    return *this;
}

node::iterator node::iterator::operator++(int)
{
    auto tmp = *this;
    this->operator++();
    return tmp;
}

node::iterator& node::iterator::operator=(iterator const& rhs)
{
    current = rhs.current;
    indices = rhs.indices;

    return *this;
}

bool node::iterator::operator!=(iterator const& rhs)
{
    return rhs.current != current;
}

node* node::iterator::next(node* n)
{
    if(n && n->children_.size())
    {
        indices.push_back(0);
        return next(n->children_[0].get());
    }
    else
        return n;
}

void node::attach(
    std::unique_ptr<node> child)
{
    child->parent = this;
    children_.push_back(std::move(child));
}

void node::on_command(
    command_t const& command,
    sf::Time const& dt)
{
    command(*this, dt);

    for(auto& child : children_)
    {
        child->on_command(command, dt);
    }
}

void node::update(
    sf::Time const& dt,
    commands_t& commands)
{
    update_self(dt, commands);

    for(auto& child : children_)
    {
        child->update(dt, commands);
    }
}

void node::sweep_removed()
{
    children_.erase(std::remove_if(children_.begin(), children_.end(), [](auto const& n){ return n->remove; }), children_.end());
    std::for_each(children_.begin(), children_.end(), std::mem_fn(&node::sweep_removed));
}

void node::draw(
    sf::RenderTarget& target,
    sf::RenderStates states) const
{
    states.transform *= getTransform();

    draw_self(target, states);

    if(sf::Keyboard::isKeyPressed(sf::Keyboard::C))
    {
        sf::FloatRect const rect = collision_bounds();

        sf::RectangleShape shape;
        shape.setPosition(sf::Vector2f(rect.left, rect.top));
        shape.setSize(sf::Vector2f(rect.width, rect.height));
        shape.setFillColor(sf::Color::Transparent);
        shape.setOutlineColor(sf::Color::Green);
        shape.setOutlineThickness(1.f);

        target.draw(shape);
    }

    for(auto& child : children_)
    {
        child->draw(target, states);
    }
}

sf::Transform node::world_transform() const
{
    auto transform = sf::Transform::Identity;

    for(scene::node const* n = this; n; n = n->parent)
    {
        transform = n->getTransform() * transform;
    }

    return transform;
}

sf::Vector2f node::world_position() const
{
    return world_transform() * sf::Vector2f{};
}

std::set<std::pair<node*, node*>> node::collisions()
{
    std::set<std::pair<node*, node*>> c;

    for(auto left = begin(), e = end(); left != e; ++left)
    {
        for(auto right = std::next(left); right != e; ++right)
        {
            if(left->collides(&(*right)))
            {
                c.insert(std::minmax(&(*left), &(*right)));
            }
        }
    }

    return c;
}

void node::draw_self(
    sf::RenderTarget&,
    sf::RenderStates) const
{}

void node::update_self(
    sf::Time const&,
    commands_t&)
{}

std::vector<node*> node::children() const
{
    std::vector<node*> c{children_.size()};

    std::transform(children_.begin(), children_.end(), c.begin(), [](auto const& up){ return up.get(); });
    
    return c;
}

bool node::collides(node const* other) const
{
    return collision_bounds().intersects(other->collision_bounds());
}

sf::FloatRect node::collision_bounds() const
{
    return {};
}

float distance(
    node const& lhs,
    node const& rhs)
{
    return utility::length(lhs.world_position() - rhs.world_position());
}

}
