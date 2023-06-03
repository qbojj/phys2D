/*
Jakub Janeczko
obiekt fizyki i odcinek
3.06.2023
*/

#include "PhysicsObject.h"

#include <glm/glm.hpp>

#include <vector>
#include <stdexcept>
#include <algorithm>

constexpr double epsilon = 1e-13;

// obrót o 90 stopni w lewo
static glm::dvec2 rot90( const glm::dvec2 &a )
{
    return glm::dvec2{ -a.y, a.x };
}

const auto &cross = vec_cross;

// > 0 jeśli (a->b)-(b->c) przechodzi odwrotnie do ruchu wskazówek zegara
// = 0 jeżeli a,b,c wspóliniowe
// < 0 w p.p.
static double ccw( const glm::dvec2 &a, const glm::dvec2 &b, const glm::dvec2 &c )
{
    const glm::dvec2 ab = b - a, cb = c - b;
    return cross( ab , cb );
}

PhysicsObject::PhysicsObject(std::vector<glm::dvec2> point_cloud, double density, uint32_t flags )
    : flags(flags)
{
    if( point_cloud.size() < 3 )
        throw std::invalid_argument( u8"Obiekt fizyki musi mieć przynajmniej 3 punkty" );
    
    // oblicz otoczkę wypukłą przez algorytm Grahma: https://en.wikipedia.org/wiki/Graham_scan
    std::vector<glm::dvec2> hull;
    
    const glm::dvec2 p0 = *std::min_element( point_cloud.begin(), point_cloud.end(),
        []( const glm::dvec2 &a, const glm::dvec2 &b ) { return a.y < b.y; } );

    // posotrój po kącie z p0 z rozstrzygnięciem remisów po długości
    std::sort( point_cloud.begin(), point_cloud.end(),
        [&p0]( const glm::dvec2 &a, const glm::dvec2 &b )
        {
            double CCW = ccw( p0, a, b );
            
            if( CCW >  epsilon ) return true;
            if( CCW < -epsilon ) return false;
            
            const glm::dvec2 pa = a - p0, pb = b - p0;

            return glm::dot(pa,pa) < glm::dot(pb,pb);
        } );
    
    for( const glm::dvec2 &point : point_cloud )
    {
        // zapewnij że punkty w otoczce tworzą wypukły obiekt
        while( hull.size() >= 2 &&
                ccw( hull[hull.size()-2], hull[hull.size()-1], point) <= epsilon )
                hull.pop_back();
        
        hull.push_back(point);
    }

    // otoczka wypukła została znaleziona

    if( hull.size() < 3 )
        throw std::runtime_error( u8"Otoczka obiektu fizyki "
            u8"musi mieć przynajmniej 3 punkty" );

    // znajdź środek tak jak zostało napisane w https://en.wikipedia.org/wiki/Centroid

    double area = 0.0;
    glm::dvec2 centroid{};

    for( size_t i = 0; i < hull.size(); i++ )
    {
        const glm::dvec2 &a = hull[i],
                         &b = hull[i == hull.size() - 1 ? 0 : i+1];

        double singed_area = cross( a, b );

        centroid += (a + b) * singed_area;
        area += singed_area;
    }

    area /= 2;
    centroid /= 6. * area;

    // oblicz masę
    center = centroid;
    inv_mass = 1.0 / (area * density);
    
    points = std::move(hull);

    // zapisz punkty względem środka
    for( glm::dvec2 &point : points )
        point -= center;

    // oblicz moment bezwładności obiektu
    // całkowity moment jest sumą momentów wszystkich trójkątów
    // https://physics.stackexchange.com/questions/708936/how-to-calculate-the-moment-of-inertia-of-convex-polygon-two-dimensions
    double momentArea = 0.0;
    
    for( size_t i = 0; i < points.size(); i++ )
    {
        const glm::dvec2 &a = points[i],
                         &b = points[i == points.size() - 1 ? 0 : i+1];
        
        momentArea += (glm::dot(a,a) + glm::dot(b,b) + glm::dot(a,b)) * cross( a, b );
    }

    momentArea /= 12.0;

    inv_moment_of_intertia = 1.0 / (momentArea * density);

    if( flags & Immovable )
    {
        // jeśli obiekt jest nieruszalny to ustaw masę i moment bezwładności na nieskończoność
        inv_mass = 0.0;
        inv_moment_of_intertia = 0.0;
    }
}

void PhysicsObject::add_impulse( glm::dvec2 point_of_application, glm::dvec2 value )
{
    velocity += value * inv_mass;
    ang_velocity += vec_cross( point_of_application, value ) * inv_moment_of_intertia;
}

void PhysicsObject::time_step( double dt )
{
    if( flags & Immovable )
    {
        velocity = glm::dvec2( 0.0 );
        ang_velocity = 0.0;
        return;
    }

    move_by( velocity * dt, ang_velocity * dt );
}

void PhysicsObject::move_by( glm::dvec2 vec, double d_angle )
{
    center += vec;
    angle += d_angle;

    // obróć punkty by odzwierciedlały prawdziwą pozycję
    double c_a = cos( d_angle ),
           s_a = sin( d_angle );

    const glm::dmat2 rot_angle{
         c_a, s_a,
        -s_a, c_a
    };

    for( glm::dvec2 &point : points )
        point = rot_angle * point;
}

Edge PhysicsObject::get_closest_edge( const glm::dvec2 &point ) const
{
    const glm::dvec2 pt = point - center;

    for( size_t i = 0; i < points.size() - 1; i++ )
    {
        // punkt pomiędzy dwoma wierzchołkami
        if( cross( points[i], pt ) >= 0 &&
            cross( pt, points[i+1] ) >= 0 )
            return { center + points[i], center + points[i+1] };
    }

    assert(
        cross( points.back(), pt ) >= 0 &&
        cross( pt, points.front() ) >= 0
    );

    return { center + points.back(), center + points.front() };
}

double Edge::signed_distance( const glm::dvec2 &p ) const
{
    return glm::dot( p - a, get_normal() );
}

bool PhysicsObject::is_point_inside_object( const glm::dvec2 &p ) const
{
    const Edge closest_edge = get_closest_edge( p );
    const double dist = closest_edge.signed_distance( p );
    return dist <= 0;
}

glm::dvec2 Edge::get_normal() const
{
    return glm::normalize( -rot90( b - a ) );
}