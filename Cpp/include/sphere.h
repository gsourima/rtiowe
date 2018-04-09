#ifndef SPHEREH
#define SPHEREH

#include <vector>

#include "hitable.h"
#include "materials.h"

class sphere: public hitable
{
    public:

        vec3 center;
        float radius;
        float inv_radius;
        material* mat;
        static std::vector<sphere*> lights;

        sphere() : hitable(), center( 0 ), radius( 1 ), inv_radius( 1 ), mat( material::default ) {}

        sphere( const vec3& c, float r, material* m );

        virtual bool hit( const ray& r, float t_min, float t_max, hit_record& rec ) const;
};

#endif
