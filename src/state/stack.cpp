#include "state/stack.h"

#include "state/state.h"

#include <deque>
#include <functional>

namespace state
{

stack::stack(
    stack::context_t context)
    : context{context}
{}

void stack::update(
    sf::Time const& dt)
{
    for (auto i = states.rbegin(); i != states.rend() && (*i)->update(dt); ++i)
    {}

    apply_pending_requests();
}

void stack::draw()
{
    for(auto& state : states)
    {
        state->draw();
    }
}

void stack::handle_event(
    sf::Event const& event)
{
    for(auto i = states.rbegin(); i != states.rend() && (*i)->handle_event(event); ++i)
    {}

       apply_pending_requests();
}

void stack::request_push(
    id const id)
{
    pending_requests.push_back([=]()
    {
        states.push_back(factory[id]());
    });
}

void stack::request_pop()
{
    pending_requests.push_back([=]()
    {
        states.pop_back();
    });
}

void stack::request_clear()
{
    pending_requests.push_back([=]()
    {
        states.clear();
    });
}

bool stack::empty() const
{
    return states.empty();
}

void stack::apply_pending_requests()
{
    while(!pending_requests.empty())
    {
        pending_requests.front()();
        pending_requests.pop_front();
    }
}

}
