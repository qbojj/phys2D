/*
Jakub Janeczko
pętla główna programu
30.05.2023
*/

#include "app.h"
#include "PhysicsObject.h"

#include <vector>
#include <chrono>

void App::run()
{
    const double
        arena_width = 1000,
        arena_height = 1000,
        wall_thickness = 1000;
    
    const double a_hw = arena_width / 2.0, // arena half width
                 a_h = arena_height,
                 w_t = wall_thickness; 

    std::vector<PhysicsObject> objs{
        PhysicsObject(
            {
                {-a_hw, 0},
                { a_hw, 0},
                { a_hw, -w_t},
                {-a_hw, -w_t}
            }, 1.0, PhysicsObject::FlagBits::Immovable ),
        PhysicsObject(
            {
                {-a_hw,       -w_t},
                {-a_hw - w_t, -w_t},
                {-a_hw,        a_h},
                {-a_hw - w_t,  a_h}
            }, 1.0, PhysicsObject::FlagBits::Immovable ),
        PhysicsObject(
            {
                { a_hw,       -w_t},
                { a_hw + w_t, -w_t},
                { a_hw,        a_h},
                { a_hw + w_t,  a_h}
            }, 1.0, PhysicsObject::FlagBits::Immovable ),
        PhysicsObject(
            {
                { a_hw + w_t,  a_h},
                {-a_hw - w_t,  a_h},
                { a_hw + w_t,  a_h + w_t},
                {-a_hw - w_t,  a_h + w_t}
            }, 1.0, PhysicsObject::FlagBits::Immovable ),
        PhysicsObject(
            {
                {-10, 5},
                { 10, 5},
                { 0, 15}
            }, 1.0, 0 ),
    };

    using namespace std::chrono;

    auto last_timestamp = high_resolution_clock::now();

    while( true )
    {
        auto cur_timestamp = high_resolution_clock::now();

        double dt = duration<double>( cur_timestamp - last_timestamp ).count();
        
        engine->onTick( objs, dt );
        if( !gui.onDraw( objs, dt ) ) break;

        last_timestamp = cur_timestamp;
    }
}