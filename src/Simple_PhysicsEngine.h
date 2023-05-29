/*
Jakub Janeczko
nagłówek klasy implementującej interfejs silnika fizyki
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

private:
    double gravity;
    
    bool is_potentially_colliding( const PhysicsObject &a, const PhysicsObject &b );
    bool handle_potential_collision( PhysicsObject &a, PhysicsObject &b );

    // get shortest distance from a's edges to b's points
    std::pair<Edge,glm::dvec2> get_shortest_edge_point_dist( 
        const PhysicsObject &a,
        const PhysicsObject &b );
    
    void deintersect_and_handle_collision(
        PhysicsObject &a, Edge edge_a,
        PhysicsObject &b, glm::dvec2 point_b );
};