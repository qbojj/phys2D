/*
Jakub Janeczko
nagłówek silnika fizyki
3.06.2023
*/

#pragma once

#include "interfaces.h"
#include "PhysicsObject.h"

#include <vector>

/**
 * @brief silnik fizyki oparty na metodzie rzutów i impulsów
 */
class Simple_PhysicsEngine : public IPhysicsEngine {
public:
    virtual void onTick( std::vector<PhysicsObject> &objs, double dt );

    double gravity = 9.81; ///< siła grawitacji [m/s2]
    double dump_velocity_factor = 0.05; ///< część prędkości jaką obiekty wytracają w 1s

    /**
     * @brief część prędkości kątowej jaką obiekty wytracają w 1s
     */
    double dump_angular_velocity_factor = 0.05;
    
    double restitution = 0.66; ///< ile energii zostaje po kolizji dwóch obiektów

    /**
     * @brief na ile części zostaje podzielona
     *        każda jednostka czasu przy symulacji
     */
    int time_subdivision = 2;

private:
    void onTick_subdivided( std::vector<PhysicsObject> &objs, double dt );

    bool is_potentially_colliding( const PhysicsObject &a, const PhysicsObject &b );
    void handle_potential_collision( PhysicsObject &a, PhysicsObject &b );

    // get shortest distance from a's edges to b's points
    std::pair<Edge,glm::dvec2> get_shortest_edge_point_dist( 
        const PhysicsObject &a,
        const PhysicsObject &b );
    
    void deintersect_and_handle_collision(
        PhysicsObject &a, PhysicsObject &b, 
        const glm::dvec2 &point, const glm::dvec2 &normal,
        double dist );
};