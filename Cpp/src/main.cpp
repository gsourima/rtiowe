#include <climits>
#include <cstdlib>
#include <ctime>
#include <cstdio>

#include "camera.h"
#include "hitable_list.h"
#include "sphere.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

float randf() { return rand() / float( RAND_MAX ); }

vec3 random_in_unit_sphere()
{
    vec3 p;
    do { p = 2 * vec3( randf(), randf(), randf() ) - 1; }
    while ( dot(p,p) >= 1 );
    return p;
}

vec3 ray_color( const ray& r, hitable* scene )
{
    hit_record rec;

    const int N = 2;

#define TMIN_EPS 0.001f

    if ( scene->hit( r, TMIN_EPS, std::numeric_limits<float>::max(), rec ) )
    {        
        // Outputting diffuse reflection
        vec3 target = rec.P + rec.N + random_in_unit_sphere();
        return 0.5f * ray_color( ray( rec.P, target-rec.P ), scene );

        // Outputting ids as colors
        // int id = 1 + rec.id;
        // return vec3( id/float(N), (N-id)/float(N), (id+N)/float(2*N) );
        
        // Outputting normal
        // return 0.5f * rec.N + 0.5f;
    }

    vec3 unit_dir = unit_vector( r.direction() );
    float t = 0.5f * unit_dir.y() + 0.5f;
    return (1.0f-t)*vec3(1.0) + t*vec3(0.5f, 0.7f, 1.0f);
}

int main()
{
    std::srand( (unsigned int) std::time(NULL));

    //int w = 240;
    //int h = 135;
    const int w = 960;
    const int h = 540;
    const int s = 32;
    const int pixels = w*h;

    char* img = new char[ w * h * 3 ];
    int p = 0;

    camera cam;
    hitable* list[2];
    list[0] = new sphere( vec3(0,0,-1), 0.5f );
    list[1] = new sphere( vec3( 0, -100.5f, -1), 100 );
    hitable* scene = new hitable_list( list, 2 );

    for ( int j = h-1; j >= 0; j-- )
    {
        for ( int i = 0; i < w; i++ )
        {
            vec3 col( 0 );

            for ( int k = 0; k < s ; k++ )
            {
                float u = float(i + randf()) / float(w);
                float v = float(j + randf()) / float(h);

                ray r = cam.get_ray(u, v);
            
                col += ray_color( r, scene );
            }
            col /= float(s);

            // Includes Gamma correction
            img[p++] = char( 255.99f*pow(col[0],0.4545f) );
            img[p++] = char( 255.99f*pow(col[1],0.4545f) );
            img[p++] = char( 255.99f*pow(col[2],0.4545f) );
        }

        if ( (j%4) == 0 )
        {
            float progress = 100 * p / float( 3 * pixels );

            printf( "Rendering in progress... [ %5.1f%% ]\r", progress );
        }
    }
    printf( "\nDone!" );

    stbi_write_png( "out.png", w, h, 3, img, 0 );
}