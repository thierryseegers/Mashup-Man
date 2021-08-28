#pragma once

#include "tomlpp.h"

#include <SFML/Graphics.hpp>

#include <array>
#include <cmath>
#include <iterator>
#include <random>
#include <string>
#include <type_traits>

namespace utility
{

// Change origin of object to be its center rather than its top-left corner.
void center_origin(
    sf::Sprite& sprite);

void center_origin(
    sf::Text& text);
    
void center_origin(
    sf::Shape& shape);

// Blends two color together by a given ratio [0., 1.].
sf::Color blend(
    sf::Color const& a,
    sf::Color const& b,
    float const ratio);

// Converts a keyboard key to its std::string representation.
std::string to_string(
    sf::Keyboard::Key const key);

template<typename T>
T to_degree(
    T const radian)
{
	return 180. / M_PI * radian;
}

template<typename T>
T to_radian(
    T const degree)
{
	return M_PI / 180. * degree;
}

float length(
    sf::Vector2f const& vector);

sf::Vector2f unit(
    sf::Vector2f const& vector);

sf::IntRect to_intrect(
    toml::array const& values);

sf::Color to_color(
    toml::array const& values);

int random(
    int const max);

// An iterator that cyles forward or barckward when it reaches the end or the beginning of its container.
template<class Container,
         class Iterator = std::conditional_t<std::is_const_v<Container>,
                                             typename Container::const_iterator,
                                             typename Container::iterator>>
struct cyclic_iterator 
{
    using iterator_category = typename Iterator::iterator_category;
    using difference_type   = typename Iterator::difference_type;
    using value_type        = typename Iterator::value_type;
    using pointer           = typename Iterator::pointer;
    using reference         = typename Iterator::reference;

    cyclic_iterator(
        Container& container,
        Iterator i)
        : container{container}
        , i{i}
    {}

    Iterator base()
    {
        return i;
    }

    cyclic_iterator& operator=(
        Iterator const other)
    {
        i = other;

        return *this;
    }

    reference operator*() const
    {
        return *i;
    }

    pointer operator->() const
    {
        return std::addressof(operator*());
    }
    
    cyclic_iterator& operator++()
    {
        if(i == std::prev(container.end()))
        {
            i = container.begin();
        }
        else
        {
            std::advance(i, 1);
        }

        return *this;
    }

    cyclic_iterator operator++(int)
    {
        cyclic_iterator tmp = *this;
        ++(*this);
        return tmp;
    }

    cyclic_iterator& operator--()
    {
        if(i == container.begin())
        {
            i = std::prev(container.end());
        }
        else
        {
            std::advance(i, -1);
        }

        return *this;
    }

    cyclic_iterator operator--(int)
    {
        cyclic_iterator tmp = *this;
        --(*this);
        return tmp;
    }

    cyclic_iterator& operator+=(
        difference_type n)
    {
        for(;n != 0; --n)
        {
            operator++();
        }
        return *this;
    }

    cyclic_iterator& operator-=(
        difference_type n)
    {
        for(;n != 0; --n)
        {
            operator--();
        }
        return *this;
    }

    friend bool operator==(
        const cyclic_iterator& a,
        const cyclic_iterator& b)
    { 
        return &a.container == &b.container && a.i == b.i;
    }

    friend bool operator!=(
        const cyclic_iterator& a,
        const cyclic_iterator& b)
    {
        return &a.container != &b.container || a.i != b.i;
    }

private:
    Container& container;
    Iterator i;
};

template<typename T>
struct reversed
{
    T& iterable;
};

template<typename T>
auto begin(
    reversed<T> r)
{
    return std::rbegin(r.iterable);
}

template<typename T>
auto end(
    reversed<T> r)
{
    return std::rend(r.iterable);
}

template<typename T>
reversed<T> reverse(
    T&& iterable)
{
    return {iterable};
}

namespace single
{

template<typename T>
T& mutable_instance()
{
    static T t;
    return t;
}

template<typename T>
T const& instance()
{
    return mutable_instance<T>();
}

}

}
