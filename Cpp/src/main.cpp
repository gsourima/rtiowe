#include "intersections.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

vec3 ray_color( const ray& r )
{
    if ( hit_sphere( vec3(0,0,-1), .5f, r ) )
        return vec3(1, 0, 0);

    vec3 unit_dir = unit_vector( r.direction() );
    float t = 0.5f * unit_dir.y() + 0.5f;
    return (1.0f-t)*vec3(1.0) + t*vec3(0.5f, 0.7f, 1.0f);
}

int main()
{
    int w = 960;
    int h = 540;

    char* img = new char[ w * h * 3 ];
    int k = 0;

    vec3 corner_ll( -2.4f, -1.35f, -1 );
    vec3 horiz( 4.8f, 0, 0 );
    vec3 vert( 0, 2.7f, 0 );
    vec3 orig( 0 );

    for ( int j = h-1; j >= 0; j-- )
    {
        for ( int i = 0; i < w; i++ )
        {
            float u = float(i) / float(w);
            float v = float(j) / float(h);

            ray r( orig, corner_ll + u*horiz + v * vert );
            
            vec3 col = ray_color( r );

            img[k++] = char( 255.99f*col[0] );
            img[k++] = char( 255.99f*col[1] );
            img[k++] = char( 255.99f*col[2] );
        }
    }

    stbi_write_png( "out.png", w, h, 3, img, 0 );
}