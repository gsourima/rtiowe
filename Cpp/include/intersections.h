#ifndef INTERSECTIONSH
#define INTERSECTIONSH

#include "ray.h"

// DEPRECATED
#include <cstdio>

float hit_sphere( const vec3& center, float radius, const ray& r )
{
    std::cout << "WARNING: using deprecated file " << __FILE__ << std::endl;
    vec3 oc = r.origin() - center;
    float a = dot( r.direction(), r.direction() );
    float b = 2 * dot( oc, r.direction() );
    float c = dot( oc, oc ) - radius * radius;
    float discriminant = b * b - 4 * a * c;
    if ( discriminant < 0 )
        return -1;
    else // return closest point
        return ( -b - sqrt( discriminant ) ) / ( 2 * a );
}

#endif
