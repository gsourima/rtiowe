#include <climits>
#include <cstdio>

#include "camera.h"
#include "hitable_list.h"
#include "sphere.h"
#include "randgen.h"
#include "materials.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#define TMIN_EPS 0.001f
#define NB_OBJS 6
#define MAX_DEPTH 8
#define SAMPLES_AA 32

vec3 ray_color( const ray& r, hitable* scene, int depth = 0 )
{
    hit_record rec;

    if ( scene->hit( r, TMIN_EPS, std::numeric_limits<float>::max(), rec ) )
    {        
        // Outputting material evaluation
        ray scattered;
        vec3 attenuation;
        if ( depth < MAX_DEPTH && rec.mat->scatter( r, rec, attenuation, scattered ) )
        {
            return attenuation * ray_color( scattered, scene, depth+1 );
        }
        else return 0;

        // Outputting diffuse reflection
        // vec3 target = rec.P + rec.N + random_in_unit_sphere();
        // return 0.5f * ray_color( ray( rec.P, target-rec.P ), scene );

        // Outputting ids as colors
        // int id = 1 + rec.id;
        // return vec3( id/float(N), (N-id)/float(N), (id+N)/float(2*N) );
        
        // Outputting normal
        // return 0.5f * rec.N + 0.5f;
    }

    vec3 unit_dir = unit_vector( r.direction() );
    float t = abs(unit_dir.y()); // 0.5f * unit_dir.y() + 0.5f;
    return t*vec3(1.0) + (1.0f-t)*vec3(0.5f, 0.7f, 1.0f);
}

int main()
{
    //int w = 240;
    //int h = 135;
    const int w = 960;
    const int h = 540;
    const int pixels = w*h;

    char* img = new char[ w * h * 3 ];
    int p = 0;

    camera cam;
    hitable* list[NB_OBJS];
    list[0] = new sphere( vec3(0,1,-3),       1.5f, new lambertian(vec3(0.1f, 0.2f, 0.5f)) );
    list[1] = new sphere( vec3(0,-100.5f,-1), 100,  new lambertian(vec3(0.6f, 0.8f, 0.2f)) );
    list[2] = new sphere( vec3(-0.8f,0,-1),      0.5f, new dielectric(1.5f) );
    list[3] = new sphere( vec3(1,0,-1),       0.5f, new metal(vec3(0.8f), 0.05f) );
    list[4] = new sphere( vec3(0.2f,-0.1f,-0.8f), 0.30f, new dielectric(1.5f) );
    list[5] = new sphere( vec3(0.2f,-0.1f,-0.8f), -0.29f, new dielectric(1.5f) );
    hitable* scene = new hitable_list( list, NB_OBJS );

    for ( int j = h-1; j >= 0; j-- )
    {
        for ( int i = 0; i < w; i++ )
        {
            vec3 col( 0 );

            for ( int k = 0; k < SAMPLES_AA ; k++ )
            {
                float u = float(i + randf()) / float(w);
                float v = float(j + randf()) / float(h);

                ray r = cam.get_ray(u, v);
            
                col += ray_color( r, scene );
            }
            col /= float(SAMPLES_AA);

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