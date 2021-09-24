#pragma once

// #include "music.h"
#include "player.h"
#include "resources.h"
#include "sound.h"
#include "state/id.h"

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include <deque>
#include <functional>
#include <map>
#include <memory>

namespace state
{

class state;

class stack
    : private sf::NonCopyable
{
public:
    struct context_t
    {
        // music::player& music;
        std::vector<std::unique_ptr<player>> players;
        sound::player& sound;
        sf::RenderWindow& window;
    } context;

    explicit stack(
        stack::context_t&& context);

    template<class State>
    void register_state(
        id const id)
    {
        factory[id] = [this]()
        {
            return std::make_unique<State>(*this);
        };
    }

    void update(
        sf::Time const& dt);

    void draw();

    void handle_event(
        sf::Event const& event);

    void request_push(
        id const id);

    void request_pop();
    
    void request_clear();

    [[nodiscard]] bool empty() const;

private:
    void apply_pending_requests();
    
    std::deque<std::unique_ptr<state>> states;
    std::deque<std::function<void ()>> pending_requests;

    std::map<id, std::function<std::unique_ptr<state> ()>> factory;
};

}
