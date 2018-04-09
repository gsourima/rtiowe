#ifndef MATERIALSH
#define MATERIALSH

#include "vec3.h"
#include "hitable.h"
#include "randgen.h"

class material
{
    public:

        vec3 emission;

        static material* default;

        material() : emission( 0 ) {}
        material( const vec3& c ): emission( c ) {}

        virtual bool scatter( const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered, vec3& direct_light, unsigned int& rays_cast ) const = 0;

    protected:

        static float inline schlick( float cosine, float ridx )
        {
            float R0 = ( 1 - ridx ) / ( 1 + ridx );
            R0 = R0 * R0;
            return R0 + ( 1 - R0 ) * pow( 1 - cosine, 5 );
        }
};

class emissive : public material
{

    public:

        emissive() : material( 1 ) {}
        emissive( const vec3& c ): material( c ) {}

        virtual bool scatter( const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered, vec3& direct_light, unsigned int& rays_cast ) const
        {
            return false;
        }
};

class lambertian : public material
{

    public:

        vec3 albedo;
        bool checker;

        lambertian( const vec3& v, bool check = false ) : albedo( v ), checker( check ) {}

        virtual bool scatter( const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered, vec3& direct_light, unsigned int& rays_cast ) const;
};

class metal : public material
{

    public:

        vec3 albedo;
        float fuzz;

        metal( const vec3& v, float f ) : albedo( v ) { if ( f < 1 ) fuzz = f; else fuzz = 1; }

        virtual bool scatter( const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered, vec3& direct_light, unsigned int& rays_cast ) const;
};

class dielectric : public material
{

    public:

        float ridx;

        dielectric( float idx ) : ridx( idx ) {}

        virtual bool scatter( const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered, vec3& direct_light, unsigned int& rays_cast ) const;
};

#endif
