#include <climits>
#include <cstdio>
#include <algorithm> // std::min

#ifdef GS_OPTIS
#include <omp.h>
#endif

#include "camera.h"
#include "hitable_list.h"
#include "materials.h"
#include "sphere.h"
#include "randgen.h"
#include "timing.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#define TMIN_EPS 0.001f
#define NB_OBJS 6
#define MAX_DEPTH 6
#define SAMPLES_AA 64

static unsigned int rays_cast = 0;

hitable* hitable::scene = NULL;

vec3 trace_ao( const ray& r, hitable* scene )
{
    hit_record rec;
    rays_cast++;

    if ( scene->hit( r, TMIN_EPS, std::numeric_limits<float>::max(), rec ) )
    {
        vec3 ao_dir = rec.N + random_in_unit_sphere();

        if ( scene->hit( ray(rec.P, ao_dir), TMIN_EPS, std::numeric_limits<float>::max(), rec ) )
            return 0;

        //else
        //{
        //    vec3 unit_dir = unit_vector( ao_dir );
        //    float t = abs(unit_dir.y()); // 0.5f * unit_dir.y() + 0.5f;
        //    return 0.6f * ( t*vec3(1.0) + (1.0f-t)*vec3(0.5f, 0.7f, 1.0f) );
        //}
    }

    return 1;
    //vec3 unit_dir = unit_vector( r.direction() );
    //float t = abs(unit_dir.y()); // 0.5f * unit_dir.y() + 0.5f;
    //return 0.6f * ( t*vec3(1.0) + (1.0f-t)*vec3(0.5f, 0.7f, 1.0f) );
}

vec3 trace_light( const ray& r, hitable* scene, int depth = 0 )
{
    hit_record rec;
    rays_cast++;

    if ( scene->hit( r, TMIN_EPS, std::numeric_limits<float>::max(), rec ) )
    {        
        // Outputting material evaluation
        ray scattered;
        vec3 attenuation;
		vec3 direct_lighting = vec3(0);
        if ( depth < MAX_DEPTH && rec.mat->scatter( r, rec, attenuation, scattered, direct_lighting, rays_cast ) )
        {
            return direct_lighting + attenuation * trace_light( scattered, scene, depth+1 );
        }
        else return rec.mat->emission;
    }

    vec3 unit_dir = unit_vector( r.direction() );
    float t = abs(unit_dir.y()); // 0.5f * unit_dir.y() + 0.5f;
    return 0.6f * ( t*vec3(1.0) + (1.0f-t)*vec3(0.5f, 0.7f, 1.0f) );
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
    rt_timer timer_total(true);
    rays_cast = 0;

    const int w = 960;
    const int h = 540;
    const int pixels = w*h;

    char* img = new char[ w * h * 3 ];

    vec3 lookfrom = vec3(5,3,4);
    vec3 lookat = 0;
    camera cam( lookfrom, lookat, vec3(0,1,0), 40, 16.0f/9.0f, 0.5f, (lookfrom-lookat).length() );

    hitable* list[NB_OBJS];
    list[0] = new sphere( vec3(0),               1, new lambertian(vec3(0.1f, 0.2f, 0.5f)) );
    list[1] = new sphere( vec3(0,-1001.1f,0), 1000, new lambertian(vec3(0.5f,0.5f,0.65f), true) );
    list[2] = new sphere( vec3(-2,0,0),          1, new dielectric(1.5f) );
    list[3] = new sphere( vec3(-2,0,0),     -0.95f, new dielectric(1.5f) );
    list[4] = new sphere( vec3(2,0,0),           1, new metal(vec3(0.8f), 0.15f) );
    list[5] = new sphere( vec3(-2,1,1.5f),    0.5f, new emissive(25*vec3(1.5f,1.2f,1)) );
	//list[5] = new sphere(vec3(0, 1, -2), 0.5f, new emissive(25 * vec3(1.5f, 1.2f, 1)));
    hitable::scene = new hitable_list( list, NB_OBJS );

    //vec3 lookfrom = vec3(13,2,3);
    //vec3 lookat = 0;
    //camera cam( lookfrom, lookat, vec3(0,1,0), 20, 16.0f/9.0f, 0.1f, 10 );

    //hitable::scene = random_scene();
    
    int nlines = 0;
    rt_timer timer_render(true);

    #ifdef GS_OPTIS
    #pragma omp parallel for
    #endif
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
            
                col += trace_light( r, hitable::scene );
                //col += trace_ao( r, hitable::scene );
            }
            col /= float(SAMPLES_AA);

            // Includes Gamma correction
            int p = 3*(w*(h-1-j)+i);
            img[p]   = char( std::min( 255.99f, 255.99f*pow(col[0],0.4545f) ) );
            img[p+1] = char( std::min( 255.99f, 255.99f*pow(col[1],0.4545f) ) );
            img[p+2] = char( std::min( 255.99f, 255.99f*pow(col[2],0.4545f) ) );
        }

        #ifdef GS_OPTIS
        #pragma omp atomic
        #endif
        nlines++;

        if ( (nlines%4) == 0 )
        {
            float progress = 100 * (nlines) / float(h);

            printf( "Rendering in progress... [ %5.1f%% ]\r", progress );
        }
    }
    timer_render.pause();

    printf( "\nDone!\n\n" );

    stbi_write_png( "out.png", w, h, 3, img, 0 );

    timer_total.pause();

    // Stats
    #ifdef GS_OPTIS
    printf( "Rendering stats [Optimized mode]\n" );
    #else
    printf( "Rendering stats\n" );
    #endif
    printf( "- Total processing time (s)... %.3lf\n", timer_total.time );
    printf( "- rendering time (s).......... %.3lf\n", timer_render.time );
    printf( "- max ray depth............... %d\n", MAX_DEPTH );
    printf( "- samples per pixel........... %d\n", SAMPLES_AA );
    printf( "- M rays cast................. %.6lf\n", 0.000001 * rays_cast );
    printf( "- M rays / sec................ %.6lf\n\n", 0.000001 * rays_cast / timer_render.time );

    system( "\"C:\\Program Files (x86)\\Google\\Picasa3\\PicasaPhotoViewer.exe\" out.png" );

    // To pause the terminal
    printf( "Press enter to exit...\n" );
    char c = std::getchar();
}