#include "PhysicsObject.h"
#include <glm/glm.hpp>
#include <vector>
#include <stdexcept>
#include <algorithm>

constexpr double epsilon = 1e-13;

// rotate 90 degrees left
static glm::dvec2 rot90( const glm::dvec2 &a )
{
    return glm::dvec2{ -a.y, a.x };
}

static double cross( const glm::dvec2 &a, const glm::dvec2 &b )
{
    return a.x*b.y - a.y*b.x;
}

// > 0 if lines (a->b)-(b->c) wind counterclockwise
// = 0 if colinear
// < 0 otherwise
static double ccw( const glm::dvec2 &a, const glm::dvec2 &b, const glm::dvec2 &c )
{
    const glm::dvec2 ab = b - a, cb = c - b;
    return cross( ab , cb );
}

PhysicsObject::PhysicsObject(std::vector<glm::dvec2> point_cloud, double density, uint32_t flags )
    : flags(flags)
{
    if( point_cloud.size() < 3 )
        throw std::invalid_argument( "Physics object must have at least 3 points" );
    
    // compute convex hull using grahm's scan: https://en.wikipedia.org/wiki/Graham_scan
    std::vector<glm::dvec2> hull;
    
    const glm::dvec2 p0 = *std::min_element( point_cloud.begin(), point_cloud.end(),
        []( const glm::dvec2 &a, const glm::dvec2 &b ) { return a.y < b.y; } );

    // sort by angle with p0 and break ties with length
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
        // ensure there are no indents in the hull
        while( hull.size() >= 2 &&
                ccw( hull[hull.size()-2], hull[hull.size()-1], point) <= epsilon )
                hull.pop_back();
        
        hull.push_back(point);
    }

    if( hull.size() < 3 )
        throw std::runtime_error( "points on physiccs object must not be colinear" );

    // our hull is complete now find centroid
    // to calculate centroid using https://en.wikipedia.org/wiki/Centroid

    double area = 0.0;
    glm::dvec2 centroid{};

    for( size_t i = 0; i < hull.size(); i++ )
    {
        const glm::dvec2 &a = hull[i],
                         &b = hull[i == hull.size() - 1 ? 0 : i+1];

        double cr = cross( a, b );

        centroid += (a + b) * cr;
        area += cr;
    }

    area /= 2;
    centroid /= 6. * area;

    // use area to compute mass
    // and centroid to get hull relative to the center
    center = centroid;
    mass = area * density;
    
    points = std::move(hull);

    for( glm::dvec2 &point : points )
        point -= center;

    // now compute moment of intertia
    // total moment of inertia is moment of intertia of all triangles
    // using https://physics.stackexchange.com/questions/708936/how-to-calculate-the-moment-of-inertia-of-convex-polygon-two-dimensions
    double MassMoment = 0.0;
    
    for( size_t i = 0; i < points.size(); i++ )
    {
        const glm::dvec2 &a = points[i],
                         &b = points[i == points.size() - 1 ? 0 : i+1];
        
        double cr = cross( a, b );

        // with thrid point at origin (0,0)
        MassMoment += cr * (glm::dot(a,a) + glm::dot(b,b) + glm::dot(a,b));
    }

    moment_of_intertia = MassMoment * density / 6.0;
}

void PhysicsObject::add_impulse( glm::dvec2 point_of_application, glm::dvec2 value )
{
    forces.push_back( { point_of_application, value } );
}

void PhysicsObject::time_step( double dt )
{
    if( flags & Immovable )
    {
        velocity = glm::dvec2( 0.0 );
        ang_velocity = 0.0;
        forces.clear();
        return;
    }

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

        // rotate 90deg left
        const glm::dvec2 poa_rot = rot90( Imp.point_of_application );

        // M * dt
        const double Mdt = glm::dot( poa_rot, Imp.value );
        dw += Mdt * inv_moment_of_intertia;
    }

    velocity += dv;
    ang_velocity += dw;

    // x' = a*dt^2 / 2 + v*dt + x
    // a = dv/dt
    // x' = dv*dt / 2 + v * dt + x
    // x' = (v + dv / 2) * dt + x
    center += (velocity + dv * 0.5) * dt;

    double diff_angle = (ang_velocity + dw * 0.5) * dt;
    angle += diff_angle;

    // apply rotation
    double c_a = cos( diff_angle ),
           s_a = sin( diff_angle );

    const glm::dmat2 rot_diff_angle{
        c_a, -s_a,
        s_a,  c_a
    };

    for( glm::dvec2 &point : points )
        point = rot_diff_angle * point;

    forces.clear();
}

Edge PhysicsObject::get_closest_edge( const glm::dvec2 &point ) const
{
    const glm::dvec2 pt = point - center;

    for( size_t i = 0; i < points.size() - 1; i++ )
    {
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

double signed_distance( const Edge &e, const glm::dvec2 &p )
{
    const glm::dvec2 pa = p - e.a,
                     normal = glm::normalize( -rot90( e.b - e.a ) );
    return glm::dot( pa, normal );
}

bool is_point_inside_object( const PhysicsObject &obj, const glm::dvec2 &p )
{
    const Edge closest_edge = obj.get_closest_edge(p);
    const double dist = signed_distance( closest_edge, p );
    return dist <= 0;
}