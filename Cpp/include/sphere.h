#ifndef SPHEREH
#define SPHEREH

#include "hitable.h"

class sphere: public hitable
{
public:
    sphere() : hitable(), center(0), radius(1) {};
    sphere( const vec3& c, float r ) : hitable(), center(c), radius(r) {};
    virtual bool hit( const ray& r, float t_min, float t_max, hit_record& rec ) const;
    vec3 center;
    float radius;
};

bool sphere::hit( const ray& r, float t_min, float t_max, hit_record& rec ) const
{
    // Note: some 2's have been simplified out
    vec3 oc = r.origin() - center;
    float a = dot( r.direction(), r.direction() );
    float b = dot( oc, r.direction() );
    float c = dot( oc, oc ) - radius*radius;

    float delta = b*b - a*c;
    
    if ( delta > 0 )
    {
        float t = ( -b - sqrt(delta) ) / a;
        if ( t < t_min || t > t_max )
        {
            t = ( -b + sqrt(delta) ) / a;
            if ( t < t_min || t > t_max )
                return false;
        }

        rec.t = t;
        rec.P = r.point_at_parameter(t);
        rec.N = ( rec.P - center ) / radius;
        rec.id = id;

        return true;
    }
    
    return false;
}

#endif
