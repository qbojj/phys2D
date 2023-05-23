#include "PhysicsObject.h"
#include <glm/glm.hpp>

void PhysicsObject::add_impulse( glm::dvec2 point_of_application, glm::dvec2 value )
{
    forces.push_back( { point_of_application, value } );
}

void PhysicsObject::time_step( double dt )
{
    glm::dvec2 dv{ 0.0 }; // diff of velocity
    double dw = 0.0; // diff of angular velocity

    const double inv_mass = 1.0 / mass;
    const double inv_moment_of_intertia = 1.0 / moment_of_intertia;

    for( const Impulse &Imp : forces )
    {
        // separate force vector into two components:
        //     parallel and perpendicular
        //      to the line between center and point of application

        const glm::dvec2 norm_poa = glm::normalize( Imp.point_of_application );

        // F = m * a ==> a = F / m  (for parallel component)
        // Imp = F * dt
        // dv = F * dt / m = Imp / m

        const glm::dvec2 parallel = glm::dot( norm_poa, Imp.value ) * norm_poa;
        dv += parallel * inv_mass;

        // M = I * dw/dt (M - moment of force)
        // dw/dt = M / I
        // M = F * r (for perpendicular component)
        // M * dt = (F * dt) * r = Imp * r
        // dw = M * dt / I

        // rotate 90deg right
        const glm::dvec2 poa_rot{ -Imp.point_of_application.y, Imp.point_of_application.x };

        // M * dt
        const double Mdt = glm::dot( poa_rot, Imp.value );
        dw += Mdt * inv_moment_of_intertia;
    }

    velocity += dv;
    ang_velocity += dw;

    center += velocity * dt;
    angle += ang_velocity * dt;

    // normalize angle
    while( angle < -M_PI ) angle += M_PI;
    while( angle >  M_PI ) angle -= M_PI;

    forces.clear();
}