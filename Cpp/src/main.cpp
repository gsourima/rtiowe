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
#define NB_OBJS 5
#define MAX_DEPTH 32
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


hitable* random_scene()
{
    int n = 500;
    hitable** list = new hitable*[n+1];
    list[0] =  new sphere(vec3(0,-1000,0), 1000, new lambertian(vec3(0.5, 0.5, 0.5), true));
    int i = 1;
    for ( int a = -11; a < 11; a++ )
    {
        for ( int b = -11; b < 11; b++ ) 
        {
            float choose_mat = randf();
            vec3 center( a + 0.9f*randf(), 0.2f, b + 0.9f*randf() );

            if ( (center-vec3(4,0.2f,0)).length() > 0.9f ) 
            { 
                if (choose_mat < 0.75f) {  // diffuse
                    list[i++] = new sphere(center, 0.2f, new lambertian(vec3(randf()*randf(), randf()*randf(), randf()*randf())));
                }
                else if (choose_mat < 0.9f) { // metal
                    list[i++] = new sphere(center, 0.2f,
                        new metal(vec3(0.5f*(1 + randf()), 0.5f*(1 + randf()), 0.5f*(1 + randf())), 0.5f*randf()));
                }
                else {  // glass
                    list[i++] = new sphere(center, 0.2f, new dielectric(1.5f));
                }
            }
        }
    }

    list[i++] = new sphere(vec3(0, 1, 0), 1.0f, new dielectric(1.5f));
    list[i++] = new sphere(vec3(-4, 1, 0), 1.0f, new lambertian(vec3(0.4f, 0.2f, 0.1f)));
    list[i++] = new sphere(vec3(4, 1, 0), 1.0f, new metal(vec3(0.7f, 0.6f, 0.5f), 0.0f));

    return new hitable_list(list,i);
}



int main()
{
    const int w = 960;
    const int h = 540;
    const int pixels = w*h;

    char* img = new char[ w * h * 3 ];
    int p = 0;

    /*
    vec3 lookfrom = vec3(5,3,4);
    vec3 lookat = 0;
    camera cam( lookfrom, lookat, vec3(0,1,0), 40, 16.0f/9.0f, 0.5f, (lookfrom-lookat).length() );

    hitable* list[NB_OBJS];
    list[0] = new sphere( vec3(0),       1, new lambertian(vec3(0.1f, 0.2f, 0.5f)) );
    list[1] = new sphere( vec3(0,-1001.1f,0), 1000,  new lambertian(vec3(0.3f, 0.5f, 0.1f), true) );
    list[2] = new sphere( vec3(-2,0,0),      1, new dielectric(1.5f) );
    list[3] = new sphere( vec3(-2,0,0),      -0.95f, new dielectric(1.5f) );
    list[4] = new sphere( vec3(2,0,0),      1, new metal(vec3(0.8f), 0.15f) );
    hitable* scene = new hitable_list( list, NB_OBJS );
    */

    vec3 lookfrom = vec3(13,2,3);
    vec3 lookat = 0;
    camera cam( lookfrom, lookat, vec3(0,1,0), 20, 16.0f/9.0f, 0.1f, 10 );

    hitable* scene = random_scene();

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