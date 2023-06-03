/*
Jakub Janeczko
implementacja silnika fizyki
3.06.2023
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

// obrót o 90 stopni w lewo
static glm::dvec2 rot90( const glm::dvec2 &v )
{
    return glm::dvec2( -v.y, v.x );
}

// znajdź AABB (axis aligned bounding box) obiektu
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

    // dwa nieruszalne obiekty nie powinny kolidować
    if( (a.flags & PhysicsObject::Immovable) &&
        (b.flags & PhysicsObject::Immovable) )
        return false;

    // czy AABB kolidują
    return glm::all(
            glm::lessThanEqual( a_min, b_max ) &&
            glm::lessThanEqual( b_min, a_max )
        );
}

// znajdź parę krawęź a - punkt b które są najbliżej
// czyli najkrótszą drogę by rozdzielić obiekt a od b
// po krawędzi a 
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

// kombinacja metod rzutowania i impulsów
// https://research.ncl.ac.uk/game/mastersdegree/gametechnologies/physicstutorials/5collisionresponse/Physics%20-%20Collision%20Response.pdf
void Simple_PhysicsEngine::deintersect_and_handle_collision(
    PhysicsObject &a, PhysicsObject &b, 
    const glm::dvec2 &point, const glm::dvec2 &normal,
    double dist )
{
    assert( !(a.flags & PhysicsObject::Immovable) ||
            !(b.flags & PhysicsObject::Immovable ) );
    
    const glm::dvec2 deintersect_vec = dist * normal;
    const double sum_inv_mass = a.inv_mass + b.inv_mass;

    a.move_by(-deintersect_vec * (a.inv_mass / sum_inv_mass), 0 );
    b.move_by( deintersect_vec * (b.inv_mass / sum_inv_mass), 0 );

    const glm::dvec2 rel_a = point - a.center;
    const glm::dvec2 rel_b = point - b.center;

    const glm::dvec2 ang_vel_a = a.ang_velocity * rot90( rel_a );
    const glm::dvec2 ang_vel_b = b.ang_velocity * rot90( rel_b );

    const glm::dvec2 full_vel_a = a.velocity + ang_vel_a;
    const glm::dvec2 full_vel_b = b.velocity + ang_vel_b;

    const glm::dvec2 contact_vel = full_vel_b - full_vel_a;

    double impulseForce = glm::dot( contact_vel, normal );

    if( impulseForce < 0 ) // obiekty rozdzielają się samoczynnie
        return;

    const double angular_force_a =
        vec_cross(rel_a, normal) * a.inv_moment_of_intertia;
    
    const glm::dvec2 inertia_a = rot90( angular_force_a * rel_a );

    const double angular_force_b =
        vec_cross(rel_b, normal) * b.inv_moment_of_intertia;
    
    const glm::dvec2 inertia_b = rot90( angular_force_b * rel_b );

    const double angular_effect = glm::dot( inertia_a + inertia_b, normal );

    const glm::dvec2 impulse = 
        ( -( 1.0 + restitution ) * impulseForce ) /
        ( sum_inv_mass + angular_effect ) * normal;

    a.add_impulse( rel_a,-impulse );
    b.add_impulse( rel_b, impulse );
}

// rozwiąż kolizję jeżeli taka zaszła
void Simple_PhysicsEngine::handle_potential_collision( 
    PhysicsObject &a, 
    PhysicsObject &b )
{
    // znajdź najkrótrzy wektor rozdzielający obiekty
    auto [e_a, p_b] = get_shortest_edge_point_dist(a, b);
    auto [e_b, p_a] = get_shortest_edge_point_dist(b, a);
    
    double d1 = e_a.signed_distance(p_b);
    double d2 = e_b.signed_distance(p_a);

    // obiekty nie nachodzą na siebie
    if( d1 > 0 || d2 > 0 ) return;

    if( d1 > d2 )
    {
        deintersect_and_handle_collision(
            a, b,
            p_b, e_a.get_normal(),
            -d1
        );
    }
    else
    {
        deintersect_and_handle_collision(
            a, b,
            p_a, e_b.get_normal(),
            d2
        );
    }
}

void Simple_PhysicsEngine::onTick( std::vector<PhysicsObject> &objs, double dt )
{
    dt /= time_subdivision;

    for( int i = 0; i < time_subdivision; i++ )
        onTick_subdivided( objs, dt );
}

void Simple_PhysicsEngine::onTick_subdivided( std::vector<PhysicsObject> &objs, double dt )
{
    // dodaj siłę grawitacji
    for( PhysicsObject &obj : objs )
        if( !( obj.flags & PhysicsObject::Immovable ) )
            obj.add_impulse( {0, 0}, { 0.0, -gravity * dt / obj.inv_mass } );

    // zmniejsz prędkości obiektów
    const double dump_vel_factor = pow( 1. - dump_velocity_factor, dt );
    const double dump_ang_vel_factor = pow( 1. - dump_angular_velocity_factor, dt );

    for( PhysicsObject &obj : objs )
    {
        obj.velocity *= dump_vel_factor;
        obj.ang_velocity *= dump_ang_vel_factor;
    }

    // przemieść obiekty zgodnie z ich prędkościami
    for( PhysicsObject &obj : objs )
        obj.time_step( dt );

    // znajdź kolizje i je rozwiąrz
    for( PhysicsObject &obj : objs )
        for( PhysicsObject &obj2 : objs )
            if( &obj != &obj2 && is_potentially_colliding( obj, obj2 ) )
                handle_potential_collision( obj, obj2 );
}