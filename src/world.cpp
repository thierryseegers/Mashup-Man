#include "world.h"

#include "command.h"
#include "configuration.h"
// #include "effects/bloom.h"
// #include "effects/post_effect.h"
// #include "entity/aircraft.h"
// #include "entity/entity.h"
// #include "entity/missile.h"
// #include "entity/projectile.h"
// #include "entity/pickup.h"
// #include "particle.h"
#include "resources.h"
#include "scene.h"
#include "utility.h"

#include <magic_enum.hpp>
#include <SFML/Graphics.hpp>
#include <spdlog/spdlog.h>

#include <algorithm>
#include <array>
#include <cmath>
#include <memory>
#include <vector>

world_t::world_t(
    sf::RenderTarget& output_target,
    sound::player& sound)
    : target{output_target}
    , view{output_target.getDefaultView()}
    , sound{sound}
    // , bounds{0.f, 0.f, view.getSize().x, 5000.f}
    // , player_spawn_point{view.getSize().x / 2.f, bounds.height - view.getSize().y / 2.f}
    // , scroll_speed{-50.f}
    // , player{nullptr}
{
    // scene_texture.create(target.getSize().x, target.getSize().y);

    // load_textures();
    build_scene();

    // view.setCenter(player_spawn_point);
}

commands_t& world_t::commands()
{
    return commands_;
}

// bool world_t::player_alive() const
// {
//     return !player->remove;
// }

// bool world_t::player_reached_end() const
// {
//     return !bounds.contains(player->getPosition());
// }

// void world_t::load_textures()
// {
//     auto& textures = utility::single::mutable_instance<resources::textures_t>();

//     textures.load(resources::texture::buttons, "Media/Textures/Buttons.png");
//     textures.load(resources::texture::entities, "Media/Textures/Entities.png");
//     textures.load(resources::texture::explosion, "Media/Textures/Explosion.png");
//     textures.load(resources::texture::finish_line, "Media/Textures/FinishLine.png");
//     textures.load(resources::texture::jungle, "Media/Textures/Jungle.png");
//     textures.load(resources::texture::particle, "Media/Textures/Particle.png");
//     textures.load(resources::texture::title_screen, "Media/Textures/TitleScreen.png");
// }

void world_t::build_scene()
{
    // Create a sound player.
    graph.attach<scene::sound_t>(sound);

    // Create layers.
    layers[magic_enum::enum_integer(layer::id::maze)] = graph.attach<layer::maze>();
    layers[magic_enum::enum_integer(layer::id::inanimates)] = graph.attach<layer::inanimates>();
    layers[magic_enum::enum_integer(layer::id::characters)] = graph.attach<layer::characters>();

    // // Create background sprite on background layer.
    // sf::Texture& background_texture = utility::single::mutable_instance<resources::textures_t>().get(resources::texture::jungle);
    // background_texture.setRepeated(true);
    // sf::IntRect const background_rect{bounds};
    // layers[layer::background]->attach<scene::sprite_t>(resources::texture::jungle, background_rect)->setPosition(bounds.left, bounds.top);

    // // Add the finish line to background layer.
    // layers[layer::background]->attach<scene::sprite_t>(resources::texture::finish_line)->setPosition(0.f, -76.f);

    // // Create the particle systems.
    // layers[layer::projectiles]->attach<particles<smoke>>(resources::textures().get(resources::texture::particle));
    // layers[layer::projectiles]->attach<particles<propellant>>(resources::textures().get(resources::texture::particle));

    // // Create leader aircraft and move to air layer.
    // player = layers[layer::aircrafts]->attach<entity::leader_t>();
    // player->setPosition(player_spawn_point);

    // auto add_enemy = [=](auto const& f, sf::Vector2f const& ds) mutable
    //     {
    //         enemy_spawns.insert({f, player_spawn_point + ds});
    //     };
    // auto const make_raptor = []
    //     {
    //         return std::make_unique<entity::raptor>();
    //     };
    // auto const make_avenger = []
    //     {
    //         return std::make_unique<entity::avenger>();
    //     };

    // add_enemy(make_raptor,  {   0.f,  -500.f});
    // add_enemy(make_raptor,  {   0.f, -1000.f});
    // add_enemy(make_raptor,  {+100.f, -1150.f});
    // add_enemy(make_raptor,  {-100.f, -1150.f});
    // add_enemy(make_avenger, {  70.f, -1500.f});
    // add_enemy(make_avenger, { -70.f, -1500.f});
    // add_enemy(make_avenger, { -70.f, -1710.f});
    // add_enemy(make_avenger, {  70.f, -1700.f});
    // add_enemy(make_avenger, {  30.f, -1850.f});
    // add_enemy(make_raptor,  { 300.f, -2200.f});
    // add_enemy(make_raptor,  {-300.f, -2200.f});
    // add_enemy(make_raptor,  {   0.f, -2200.f});
    // add_enemy(make_raptor,  {   0.f, -2500.f});
    // add_enemy(make_avenger, {-300.f, -2700.f});
    // add_enemy(make_avenger, {-300.f, -2700.f});
    // add_enemy(make_raptor,  {   0.f, -3000.f});
    // add_enemy(make_raptor,  { 250.f, -3250.f});
    // add_enemy(make_raptor,  {-250.f, -3250.f});
    // add_enemy(make_avenger, {   0.f, -3500.f});
    // add_enemy(make_avenger, {   0.f, -3700.f});
    // add_enemy(make_raptor,  {   0.f, -3800.f});
    // add_enemy(make_avenger, {   0.f, -4000.f});
    // add_enemy(make_avenger, {-200.f, -4200.f});
    // add_enemy(make_raptor,  { 200.f, -4200.f});
    // add_enemy(make_raptor,  {   0.f, -4400.f});
}

// void world_t::remove_unviewables()
// {
//     commands_.push(make_command<entity::entity<>>([battlefield = battlefield_bounds()](entity::entity<>& e, sf::Time const&)
//         {
//             if(!battlefield.intersects(e.collision_bounds()))
//             {
//                 e.remove = true;
//             }
//         }));
// }

// void world_t::spawn_enemies()
// {
//     while(!enemy_spawns.empty() &&
//           enemy_spawns.rbegin()->where.y > battlefield_bounds().top)
//     {
//         auto const enemy_spawn = enemy_spawns.extract(*enemy_spawns.rbegin()).value();

//         auto e = enemy_spawn.what();
//         e->setPosition(enemy_spawn.where);
//         e->setRotation(180.f);

//         layers[aircrafts]->attach(std::move(e));
//     }
// }

template<typename Entity1, typename Entity2>
std::pair<Entity1*, Entity2*> match(std::pair<scene::node*, scene::node*> const& p)
{
    if(auto *pa = dynamic_cast<Entity1*>(p.first))
    {
        if(auto *pb = dynamic_cast<Entity2*>(p.second))
        {
            return {pa, pb};
        }
    }
    else if(auto *pa = dynamic_cast<Entity2*>(p.first))
    {
        if(auto *pb = dynamic_cast<Entity1*>(p.second))
        {
            return {pb, pa};
        }
    }
    
    return {nullptr, nullptr};
}

// void world_t::handle_collisions()
// {
//     for(auto const& collision : graph.collisions())
//     {
//         if(auto [aircraft, projectile] = match<entity::hostile<entity::aircraft_t>, entity::friendly<entity::projectile>>(collision); aircraft && projectile)
//         {
//             spdlog::info("Friendly shot a hostile!");
//             aircraft->damage(projectile->damage);
//             projectile->remove = true;
//         }
//         else if(auto [aircraft, projectile] = match<entity::friendly<entity::aircraft_t>, entity::hostile<entity::projectile>>(collision); aircraft && projectile)
//         {
//             spdlog::info("Friendly got shot!");
//             aircraft->damage(projectile->damage);
//             projectile->remove = true;
//         }
//         else if(auto [leader, pickup] = match<entity::leader_t, entity::pickup::pickup>(collision); leader && pickup)
//         {
//             spdlog::info("Leader got pickup!");
//             pickup->apply(*leader);
//             pickup->remove = true;
//             leader->play_local_sound(commands_, resources::sound_effect::collect_pickup);
//         }
//         else if(auto [leader, enemy] = match<entity::leader_t, entity::enemy>(collision); leader && enemy)
//         {
//             spdlog::info("Leader crashed into enemy!");
//             auto const leader_health = leader->health();
//             leader->damage(enemy->health());
//             enemy->damage(leader_health);
//         }
//     }
// }

// void world_t::guide_missiles()
// {
//     // Setup command that stores all enemies in container.
//     commands_.push(make_command<entity::enemy>([=](entity::enemy& e, sf::Time const&)
//         {
//             enemies.push_back(&e);
//         }));

//     commands_.push(make_command<entity::guided_missile<entity::friendly>>([=](entity::guided_missile<entity::friendly>& gm, sf::Time const&)
//         {
//             if(!enemies.empty())
//             {
//                 auto e =std::min_element(enemies.begin(), enemies.end(), [&](entity::enemy* e1, entity::enemy* e2)
//                     {
//                         return scene::distance(gm, *e1) < scene::distance(gm, *e2);
//                     });

//                 gm.guide((*e)->world_position());
//             }
//         }));

//     enemies.clear();
// }

// sf::FloatRect world_t::view_bounds() const
// {
//     return sf::FloatRect(view.getCenter() - view.getSize() / 2.f, view.getSize());
// }

// sf::FloatRect world_t::battlefield_bounds() const
// {
//     // Return view bounds + some area at top, where enemies spawn.
//     sf::FloatRect bounds = view_bounds();
//     bounds.top -= 100.f;
//     bounds.height += 100.f;

//     return bounds;
// }

void world_t::update(
    sf::Time const dt)
{
    // // Scroll the view.
    // view.move(0.f, scroll_speed * dt.asSeconds());

    // // Flag entities outside viewable area.
    // remove_unviewables();

    // // Guide guided missiles.
    // guide_missiles();


    // // Reset player velocity.
    // player->velocity = {0.f, 0.f};

    // Dispatch commands.
    while(!commands_.empty())
    {
        graph.on_command(commands_.front(), dt);
        commands_.pop();
    }

    // // Adjust player velocity if it is flying diagonally.
    // auto const velocity = player->velocity;
    // if(velocity.x != 0.f && velocity.y != 0.f)
    // {
    //     player->velocity /= std::sqrt(2.f);
    // }
    // player->velocity += {0.f, scroll_speed};

    // // Prevent the player from going off-screen.
    // sf::FloatRect const bounds{view.getCenter() - view.getSize() / 2.f, view.getSize()};
    // float const border_distance = 40.f;

    // auto position = player->getPosition();
    // position.x = std::clamp(position.x, bounds.left + border_distance, bounds.left + bounds.width - border_distance);
    // position.y = std::clamp(position.y, bounds.top + border_distance, bounds.top + bounds.height - border_distance);

    // player->setPosition(position);

    // // Deal with collision.
    // handle_collisions();

    // // Remove all destroyed entities, spawn new enemies if need be.
    // graph.sweep_removed();
    // spawn_enemies();

    // Remove played sounds and reposition player in sound space.
    sound.remove_stopped();
    // sound.listener_position(player->world_position());

    // Update the entire graph.
    graph.update(dt, commands_);
}

void world_t::draw()
{
    // if(effect::post_effect::supported())
    // {
    //     scene_texture.clear();
    //     scene_texture.setView(view);
    //     scene_texture.draw(graph);
    //     scene_texture.display();
    //     bloom_effect.apply(scene_texture, target);
    // }
    // else
    // {
        target.setView(view);
        target.draw(graph);
    // }
}
