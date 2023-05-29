/*
Jakub Janeczko
implementacja silnika fizyki
28.05.2023
*/

#include "Simple_PhysicsEngine.h"
#include "PhysicsObject.h"

#include <glm/glm.hpp>

#include <vector>
#include <algorithm>
#include <limits>
#include <numeric>
#include <functional>
#include <assert.h>

// get axis aligned bounding box of an object
static std::pair<glm::dvec2, glm::dvec2> get_aabb( 
    const PhysicsObject &obj )
{
    const auto v_min = []( const glm::dvec2 &v1, const glm::dvec2 &v2 )
    {
        return glm::min( v1, v2 );
    };

    const auto v_max = []( const glm::dvec2 &v1, const glm::dvec2 &v2 )
    {
        return glm::max( v1, v2 );
    };

    const double inf = std::numeric_limits<double>::infinity();

    return std::make_pair(
        obj.center + std::reduce( 
            obj.points.begin(), obj.points.end(), glm::dvec2{  inf,  inf }, v_min),

        obj.center + std::reduce( 
            obj.points.begin(), obj.points.end(), glm::dvec2{ -inf, -inf }, v_max)
    );
}

bool Simple_PhysicsEngine::is_potentially_colliding( 
    const PhysicsObject &a,
    const PhysicsObject &b )
{
    auto [a_min, a_max] = get_aabb(a);
    auto [b_min, b_max] = get_aabb(b);

    // two immovable object should not collide
    if( (a.flags & PhysicsObject::Immovable) &&
        (b.flags & PhysicsObject::Immovable) )
        return false;

    // are AABBs intersecting?
    return glm::all(
            glm::lessThanEqual( a_min, b_max ) &&
            glm::lessThanEqual( b_min, a_max )
        );
}

std::pair<Edge,glm::dvec2>
Simple_PhysicsEngine::get_shortest_edge_point_dist( 
    const PhysicsObject &a,
    const PhysicsObject &b )
{
    std::vector<glm::dvec2> world_a, world_b;
    for( const glm::dvec2 &p : a.points )
        world_a.push_back( p + a.center );
    
    for( const glm::dvec2 &p : b.points )
        world_b.push_back( p + b.center );

    const auto get_closest_b_point = [&]( const Edge &e ) -> glm::dvec2
    {
        return *std::min_element(world_b.begin(), world_b.end(),
            [&]( const glm::dvec2 &pt1, const glm::dvec2 &pt2 )
            {
                return e.signed_distance(pt1) < e.signed_distance(pt2);
            } );
    };

    Edge edge{ world_a.back(), world_a.front() };
    glm::dvec2 pt = get_closest_b_point( edge );

    for( size_t i = 0; i < world_a.size() - 1; i++ )
    {
        Edge new_edge{ world_a[i], world_a[i+1] };
        glm::dvec2 new_pt = get_closest_b_point( new_edge );

        if( new_edge.signed_distance(new_pt) >
            edge.signed_distance(pt) )
        {
            edge = new_edge;
            pt = new_pt;   
        }
    }

    return {edge, pt};
}


void Simple_PhysicsEngine::deintersect_and_handle_collision(
    PhysicsObject &a, Edge edge_a,
    PhysicsObject &b, glm::dvec2 point_b )
{
    double dist = edge_a.signed_distance( point_b );
    assert( dist <= 0 ); // intersecting
    assert( !(a.flags & PhysicsObject::Immovable) ||
            !(b.flags & PhysicsObject::Immovable ) );
    
    glm::dvec2 edge_dir = glm::normalize( edge_a.b - edge_a.a );
    glm::dvec2 edge_normal{ edge_dir.y, -edge_dir.x };

    glm::dvec2 deintersect_vec = dist * edge_normal;

    glm::dvec2 closest_on_edge = edge_a.point_closest( point_b );

    glm::dvec2 rel_a = closest_on_edge - a.center;
    glm::dvec2 rel_b = point_b - b.center;

    double mass_ratio; // a to (a+b)

    if( a.flags & PhysicsObject::Immovable )
        mass_ratio = 1;
    else if( b.flags & PhysicsObject::Immovable )
        mass_ratio = 0;
    else
        mass_ratio = a.mass / (a.mass + b.mass);

    a.move_by( deintersect_vec * (1 - mass_ratio), 0 );
    b.move_by(-deintersect_vec * mass_ratio, 0 );
    
    // energy calculations... ????
    a.add_impulse( rel_a, deintersect_vec * 1e4 );
    b.add_impulse( rel_b,-deintersect_vec * 1e4 );
}

// handle collision if it occured
// and return true if it did so
bool Simple_PhysicsEngine::handle_potential_collision( 
    PhysicsObject &a, 
    PhysicsObject &b )
{
    // find shortest de-intersect vector
    auto [e_a, p_b] = get_shortest_edge_point_dist(a, b);
    auto [e_b, p_a] = get_shortest_edge_point_dist(b, a);
    
    double d1 = e_a.signed_distance(p_b);
    double d2 = e_b.signed_distance(p_a);

    if( d1 > 0 || d2 > 0 ) return false;

    if( d1 > d2 )
        deintersect_and_handle_collision(
            a, e_a,
            b, p_b );
    else
        deintersect_and_handle_collision(
            b, e_b,
            a, p_a );

    return true;
}

void Simple_PhysicsEngine::onTick( std::vector<PhysicsObject> &objs, double dt )
{
    // apply gravity
    for( PhysicsObject &obj : objs )
        obj.add_impulse( {0, 0}, { 0.0, -gravity * dt * obj.mass } );

    // resolve moves from last time-step
    for( PhysicsObject &obj : objs )
        obj.time_step( dt );

    for( PhysicsObject &obj : objs )
        for( PhysicsObject &obj2 : objs )
            if( &obj != &obj2 && is_potentially_colliding( obj, obj2 ) )
                handle_potential_collision( obj, obj2 );
}