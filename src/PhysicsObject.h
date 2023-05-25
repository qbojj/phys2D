#pragma once
#include <glm/glm.hpp>
#include <vector>

// represents edge from a to b
struct Edge
{
    glm::dvec2 a, b;
};

// PhysicsObject
// contains all information an object would need to perform physics calcutations
// unit of length is meters
// unit of mass is kg
// unit of time is second
// angle is represendted in radians
class PhysicsObject {
public:
    // create a physics object from point cloud and density
    //    makes convex hull of point cloud and winds it counterclockwise
    //    then computes mass, center and moment of inertia from
    //    the convex hull and density
    // if there are less than 3 points in point_cloud invalid_argument is thrown
    PhysicsObject(std::vector<glm::dvec2> point_cloud, double density, uint32_t flags = 0 );

    std::vector<glm::dvec2> points; // offsets to center
    double mass, moment_of_intertia;
    
    glm::dvec2 center, velocity; // cenroid of object and object velocity
    double angle, ang_velocity; // accumulated angle and angular velocity

    enum FlagBits {
        Immovable = 1 << 0
    };

    uint32_t flags;

    // add impulse to object queue
    void add_impulse( glm::dvec2 point_of_application, glm::dvec2 value );
    
    // applies impulses and moves dt seconds in time
    void time_step( double dt );

    // returns closest edge to the point
    Edge get_closest_edge( const glm::dvec2 &point ) const;

private:
    struct Impulse {
        glm::dvec2 point_of_application; // relative to the center
        glm::dvec2 value;
    };

    std::vector<Impulse> forces;
};

// computes signed distance from line
double signed_distance( const Edge &e, const glm::dvec2 &p );

bool is_point_inside_object( const PhysicsObject &obj, const glm::dvec2 &p );