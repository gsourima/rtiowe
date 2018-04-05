#ifndef MATERIALSH
#define MATERIALSH

#include "vec3.h"
#include "hitable.h"
#include "randgen.h"

class material
{
public:
    virtual bool scatter( const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered ) const = 0;

    static material* default;
};

class lambertian : public material
{

public:

    vec3 albedo;

    lambertian( const vec3& v ) : albedo(v) {}

    virtual bool scatter( const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered ) const
    {
        scattered = ray( rec.P, rec.N + random_in_unit_sphere() );

        attenuation = albedo;

        return true;
    }
};

class metal : public material
{

public:

    vec3 albedo;
    float fuzz;

    metal( const vec3& v, float f ) : albedo(v) { if (f < 1) fuzz = f; else fuzz = 1; }

    virtual bool scatter( const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered ) const
    {
        vec3 reflected = r_in.direction().reflect(rec.N) + fuzz*random_in_unit_sphere();

        scattered = ray( rec.P, unit_vector(reflected) );

        attenuation = albedo;

        return ( dot( scattered.direction(), rec.N ) > 0 );
    }
};

material* material::default = new lambertian( 1 );

#endif
