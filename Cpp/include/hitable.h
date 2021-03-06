#ifndef HITABLEH
#define HITABLEH

#include "ray.h"

class material;

static int object_id_cpt = 0;

struct hit_record
{
    float t;
    vec3 P;
    vec3 N;
    int id;
    material* mat;
};

class hitable
{
    public:
        hitable() { id = object_id_cpt++; }
        virtual bool hit( const ray& r, float t_min, float t_max, hit_record& rec ) const = 0;
        int id;

        static hitable* scene;
};

#endif
