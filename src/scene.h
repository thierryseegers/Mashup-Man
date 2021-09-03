#pragma once

#include "command.h"
#include "resources.h"
#include "sound.h"

#include <SFML/Graphics.hpp>
#include <SFML/System/Time.hpp>

#include <array>
#include <deque>
#include <memory>
#include <set>
#include <utility>
#include <vector>

namespace scene
{

class node :
    public sf::Transformable,
    public sf::Drawable,
    private sf::NonCopyable
{
public:
    class iterator
    {
    public:
        using iterator_category = std::input_iterator_tag;
        using value_type = node;
        using reference = node&;
        using pointer = node*;
        using difference_type = std::ptrdiff_t;

        iterator(
            node* root = nullptr);

        reference operator*();

        pointer operator->();

        iterator operator++();

        iterator operator++(int);

        iterator& operator=(iterator const& rhs);

        [[nodiscard]] bool operator!=(iterator const& rhs);

    private:
        node* next(node* n);

        std::deque<size_t> indices;
        node* current;
    };

    virtual ~node() = default;

    iterator begin()
    {
        return {this};
    }

    iterator end() const
    {
        return {};
    }

    void attach(
        std::unique_ptr<node> child);

    template<typename Node, class... Args>
    Node* attach(
        Args&&... args)
    {
        auto& child = children_.emplace_back(new Node(std::forward<Args>(args)...));
        child->parent = this;
        return static_cast<Node*>(child.get());
    }

    void on_command(
        command_t const& command,
        sf::Time const& dt);

    void update(
        sf::Time const& dt,
        commands_t& commands);

    void sweep_removed();

    virtual void draw(
        sf::RenderTarget& target,
        sf::RenderStates states) const final;

    std::set<std::pair<node*, node*>> collisions();

    std::vector<node*> children() const;

    [[nodiscard]] bool collides(node const* other) const;

    sf::Transform world_transform() const;

    sf::Vector2f world_position() const;

    bool remove = false;

protected:
    friend class const_iterator;
    
    virtual void update_self(
        sf::Time const& dt,
        commands_t& commands);

    virtual void draw_self(
        sf::RenderTarget& target,
        sf::RenderStates states) const;

    virtual sf::FloatRect collision_bounds() const;

    std::vector<std::unique_ptr<node>> children_;
    node *parent = nullptr;
};

bool collision(
    node const& lhs,
    node const& rhs);

float distance(
    node const& lhs,
    node const& rhs);

template<size_t Count>
using layers = std::array<scene::node*, Count>;

class sound_t :
    public node
{
public:
    explicit sound_t(
        sound::player& sound)
        : sound{sound}
    {}

    void play(
        resources::sound_effect const se)
    {
        sound.play(se);
    }

    void play(
        resources::sound_effect const se,
        sf::Vector2f const position)
    {
        sound.play(se, position);
    }

private:
    sound::player& sound;
};

}
