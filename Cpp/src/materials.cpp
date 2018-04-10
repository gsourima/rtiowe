#include "materials.h"

#include <cmath>
#include <algorithm> // std::max

#include "sphere.h"

#define SAMPLE_LIGHTS

material* material::default = new lambertian( 1 );

bool lambertian::scatter( const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered, vec3& direct_light, unsigned int& rays_cast ) const
{
    vec3 dir = rec.N + random_unit_dir();
    scattered.set( rec.P, dir );

    attenuation = albedo;

    if ( checker )
    {
        vec3 v = 2 * ( 100000 + rec.P );
        int idx = int( v.x() ) + int( v.z() ) + int( v.y() );
        if ( ( idx % 2 ) == 0 )
            attenuation *= 0.5f;
    }

    #ifdef SAMPLE_LIGHTS
    hit_record rec_light;

    for each ( sphere * sphere_light in sphere::lights )
    {
        const vec3& o = rec.P;
        const vec3& c = sphere_light->center;
        const float r = sphere_light->radius;

        vec3 oc = c - o;
        vec3 w = unit_vector( oc );
        vec3 u = unit_vector( cross( fabs(w.x())>.01f ? vec3(0,1,0) : vec3(1,0,0), w ) );
        vec3 v = cross( w, u );
        float cosAMax = sqrtf( 1 - r * r / oc.squared_length() );
        float eps1 = randf(), eps2 = randf();
        float cosA = 1 - eps1 + eps1 * cosAMax;
        float sinA = sqrtf( 1 - cosA * cosA );
        float phi  = 2 * float(M_PI) * eps2;
        dir = unit_vector( u * cosf(phi) * sinA + v * sinf(phi) * sinA + w * cosA );

        ray light_ray( o, dir );

        #pragma omp atomic
        rays_cast++;

        if ( hitable::scene->hit( light_ray, 0.001f, 10000.0f, rec_light ) )
        {
            if ( rec_light.id == sphere_light->id )
            {
                float inv_pdf_simp = 2 * ( 1 - cosAMax );
                vec3 nl = dot( rec.N, r_in.dirref() ) < 0 ? rec.N : -rec.N;
                direct_light += attenuation * sphere_light->mat->emission * std::max( 0.0f, dot( dir, nl ) ) * inv_pdf_simp;
            }
        }
    }
    #endif

    return true;
}

bool metal::scatter( const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered, vec3& direct_light, unsigned int& rays_cast ) const
{
    vec3 reflected = unit_vector( r_in.dirref() ).reflect( rec.N ) + fuzz * random_unit_dir();

    scattered.set( rec.P, unit_vector( reflected ) );

    attenuation = albedo;

    return ( dot( scattered.dirref(), rec.N ) > 0 );
}

bool dielectric::scatter( const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered, vec3& direct_light, unsigned int& rays_cast ) const
{
    vec3 outward_normal;
    vec3 reflected = unit_vector( r_in.dirref() ).reflect( rec.N );
    vec3 refracted;
    float ni_over_nt;
    float reflect_prob;
    float cosine;
    float din = dot( r_in.dirref(), rec.N );
    attenuation = vec3( 1 );

    if ( din > 0 )
    {
        outward_normal = -rec.N;
        ni_over_nt = ridx;
        cosine = ridx * din / r_in.dirref().length();
    }
    else
    {
        outward_normal = rec.N;
        ni_over_nt = 1 / ridx;
        cosine = -din / r_in.dirref().length();
    }

    if ( r_in.dirref().refract( outward_normal, ni_over_nt, refracted ) )
    {
        reflect_prob = schlick( cosine, ridx );
    }
    else
    {
        reflect_prob = 1;
    }

    if ( randf() < reflect_prob )
        scattered.set( rec.P, reflected );
    else
        scattered.set( rec.P, refracted );

    return true;
}