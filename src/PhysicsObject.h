#pragma once
#include <glm/glm.hpp>
#include <vector>

class PhysicsObject {
public:
    std::vector<glm::dvec2> points; // offsets to center before rotation
    double mass, moment_of_intertia;
    
    glm::dvec2 center, velocity;
    double angle, ang_velocity;

    // add impulse to object queue
    void add_impulse( glm::dvec2 point_of_application, glm::dvec2 value );
    
    // applies impulses and moves dt seconds in time
    void time_step( double dt );

private:
    struct Impulse {
        glm::dvec2 point_of_application; // relative to the center
        glm::dvec2 value;
    };

    std::vector<Impulse> forces;
};