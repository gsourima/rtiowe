#ifndef RANDGENH
#define RANDGENH

#include <cstdlib>
#include <ctime>

#include "vec3.h"

static void rand_init() { std::srand( (unsigned int) std::time(NULL)); }

static float randf() { return rand() / float( RAND_MAX ); }

static vec3 random_in_unit_sphere()
{
    vec3 p;
    do { p = 2 * vec3( randf(), randf(), randf() ) - 1; }
    while ( dot(p,p) >= 1 );
    return p;
}

static vec3 random_in_unit_disk()
{
    vec3 p;
    do { p = 2 * vec3( randf(), randf(), 0 ) - vec3(1,1,0); }
    while ( dot(p,p) >= 1 );
    return p;
}

#endif
