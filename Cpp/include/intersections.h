#ifndef INTERSECTIONSH
#define INTERSECTIONSH

#include "ray.h"

bool hit_sphere( const vec3& center, float radius, const ray& r )
{
    vec3 oc = r.origin() - center;
    float a = dot( r.direction(), r.direction() );
    float b = 2 * dot( oc, r.direction() );
    float c = dot( oc, oc ) - radius*radius;
    float discriminant = b*b - 4*a*c;
    return ( discriminant >= 0 );
}

#endif
