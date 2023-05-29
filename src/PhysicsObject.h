/*
Jakub Janeczko
nagłówek klasy obiektu fizyki
28.05.2023
*/

#pragma once

#include <glm/glm.hpp>

#include <vector>

/**
 * \brief directed edge from a to b
 */
class Edge
{
public:
    glm::dvec2 a, b;

    double signed_distance( const glm::dvec2 &p ) const;
    
    glm::dvec2 point_closest( const glm::dvec2 &p ) const;
};

/**
 * \brief object used in physics calculations
 * 
 * contains all information an object would need to perform physics calcutations
 * 
 * unit of length is meters
 * unit of mass is kg
 * unit of time is second
 * angle is represendted in radians
 */
class PhysicsObject {
public:
    /**
     * \brief create a physics object from point cloud and density   
     * 
     * by making a convex hull of the point cloud and winding it counterclockwise
     * and computing mass, center and moment of inertia
     * 
     * if there are less than 3 points in point_cloud invalid_argument is thrown
     */
    PhysicsObject(std::vector<glm::dvec2> point_cloud, double density, uint32_t flags = 0 );

    std::vector<glm::dvec2> points; ///< offsets to center
    double mass, moment_of_intertia;
    
    glm::dvec2 center; ///< location of the cenroid of the object
    glm::dvec2 velocity;
    double angle; ///< accumulated angle
    double ang_velocity; ///< angular velocity

    /**
     * flags for PhysicsObjects
     */
    enum FlagBits {
        Immovable = 1 << 0 ///< object should never change its position
    };

    uint32_t flags; ///< flags, contains 0 or more FlagBits

    /**
     * add impulse to the object
     */
    void add_impulse( glm::dvec2 point_of_application, glm::dvec2 value );
    
    /**
     * \brief applies impulses and moves dt seconds in time
     * 
     * in accordance to its linear and angular velocity and difference of time
     */
    void time_step( double dt );

    /**
     * \brief move object by a vector and angle
     */
    void move_by( glm::dvec2 vec, double angle );


    /**
     * get the closest edge to the point
     */
    Edge get_closest_edge( const glm::dvec2 &point ) const;

    /**
     * check if point is inside of the object
     */
    bool is_point_inside_object( const glm::dvec2 &p ) const;
};