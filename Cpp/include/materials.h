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

class dielectric : public material
{

public:

    float ridx;

    dielectric( float idx ) : ridx(idx) {}

    virtual bool scatter( const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered ) const
    {
        vec3 outward_normal;
        vec3 reflected = unit_vector( r_in.direction().reflect(rec.N) );
        vec3 refracted;
        float ni_over_nt;
        float reflect_prob;
        float cosine;
        float din = dot( r_in.direction(), rec.N );
        attenuation = vec3( 1 );

        if ( din > 0 )
        {
            outward_normal = -rec.N;
            ni_over_nt = ridx;
            cosine = ridx * din / r_in.direction().length();
        }
        else
        {
            outward_normal = rec.N;
            ni_over_nt = 1/ ridx;
            cosine = -din / r_in.direction().length();
        }

        if ( r_in.direction().refract( outward_normal, ni_over_nt, refracted ) )
        {
            reflect_prob = schlick( cosine, ridx );
        }
        else
        {
            reflect_prob = 1;
        }

        if ( randf() < reflect_prob )
            scattered = ray( rec.P, reflected );
        else
            scattered = ray( rec.P, refracted );

        return true;
    }

private:

    static float schlick( float cosine, float ridx )
    {
        float R0 = (1-ridx) / (1+ridx);
        R0 = R0*R0;
        return R0 + (1-R0) * pow( 1-cosine, 5 );
    }
};

material* material::default = new lambertian( 1 );

#endif
