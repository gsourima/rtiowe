#ifndef CAMERAH
#define CAMERAH

#define _USE_MATH_DEFINES
#include <cmath>

#include "ray.h"
#include "randgen.h"

class camera
{
public:
    camera( vec3 lookfrom, vec3 lookat, vec3 vup, float vfov, float aspect, float aperture = 0, float focus_dist = 1 )
    {
        lens_radius = 0.5f * aperture;
        focus_distance = focus_dist;

        set_pose( lookfrom, lookat, vup );
        set_vfov( vfov, aspect );
        update_screen();
    }

    void set_pose( const vec3& lookfrom, const vec3& lookat, const vec3& vup )
    {
        origin = lookfrom;
        w = unit_vector( lookfrom - lookat );
        u = unit_vector( cross( vup, w ) );
        v = cross( w, u );
    }

    void set_vfov( float vfov, float aspect )
    {
        float theta = vfov * float(M_PI) / 180.0f;
        half_height = tan( 0.5f * theta );
        half_width = half_height * aspect;
    }

    void update_screen()
    {
        corner_ll = origin - focus_distance * ( half_width * u + half_height * v + w );
        horiz = 2 * half_width * u * focus_distance;
        vert = 2 * half_height * v * focus_distance;
    }

    ray get_ray( float s, float t )
    {
        vec3 rd = lens_radius * random_in_unit_disk();
        vec3 offset = u * rd.x() + v * rd.y();
        return ray( origin + offset, corner_ll + s*horiz + t*vert - origin - offset );
    }

    // Lens params
    float lens_radius;
    float focus_distance;
    // Pose params
    vec3 origin;
    vec3 u;
    vec3 v;
    vec3 w;
    // Fov params
    float half_width;
    float half_height;
    // Screen params
    vec3 corner_ll;
    vec3 horiz;
    vec3 vert;
};

#endif
