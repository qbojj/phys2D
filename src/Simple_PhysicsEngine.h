/*
Jakub Janeczko
nagłówek silnika fizyki
28.05.2023
*/

#pragma once

#include "interfaces.h"
#include "PhysicsObject.h"

#include <vector>

class Simple_PhysicsEngine : public IPhysicsEngine {
public:
    Simple_PhysicsEngine( double gravity ) : gravity(gravity) {}
    void onTick( std::vector<PhysicsObject> &objs, double dt ) override;

    double gravity = 9.81;
    double dump_velocity_factor = 0.05;
    double dump_angular_velocity_factor = 0.05;
    double restitution = 0.66;

    int time_subdivision = 2;

private:
    void onTick_subdivided( std::vector<PhysicsObject> &objs, double dt );

    bool is_potentially_colliding( const PhysicsObject &a, const PhysicsObject &b );
    bool handle_potential_collision( PhysicsObject &a, PhysicsObject &b );

    // get shortest distance from a's edges to b's points
    std::pair<Edge,glm::dvec2> get_shortest_edge_point_dist( 
        const PhysicsObject &a,
        const PhysicsObject &b );
    
    void deintersect_and_handle_collision(
        PhysicsObject &a, PhysicsObject &b, 
        const glm::dvec2 &point, const glm::dvec2 &normal,
        double dist );
};