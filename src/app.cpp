#include "app.h"
#include "PhysicsObject.h"
#include <vector>
#include <chrono>

void App::run()
{
    std::vector<PhysicsObject> objs{
        PhysicsObject(
            {
                {-100, 0},
                { 100, 0},
                { 100, -100},
                {-100, -100}
            }, 1.0, PhysicsObject::FlagBits::Immovable )
    };

    using namespace std::chrono;

    auto last_timestamp = high_resolution_clock::now();

    while( true )
    {
        auto cur_timestamp = high_resolution_clock::now();

        double dt = duration<double>( cur_timestamp - last_timestamp ).count();

        for( PhysicsObject &obj : objs )
            obj.time_step( dt );
        
        engine->onTick( objs, dt );

        if( !gui->onDraw( objs, dt ) ) break;

        last_timestamp = cur_timestamp;
    }
}