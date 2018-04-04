#ifndef CAMERAH
#define CAMERAH

#include "ray.h"

class camera
{
public:
    camera()
    {
        // 16:9 ratio
        corner_ll = vec3( -2.4f, -1.35f, -1 );
        horiz = vec3( 4.8f, 0, 0 );
        vert = vec3( 0, 2.7f, 0 );
        orig = vec3( 0 );
    }

    ray get_ray( float u, float v )
    {
        return ray( orig, corner_ll + u*horiz + v*vert - orig );
    }

    vec3 corner_ll;
    vec3 horiz;
    vec3 vert;
    vec3 orig;
};

#endif
