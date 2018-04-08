#ifndef RANDGENH
#define RANDGENH

#include <cstdlib>
#include <ctime>

#include "vec3.h"

static const float kInvRandMax = 1.0f / float(RAND_MAX);

static void rand_init() { std::srand( (unsigned int) 42 ); }

static uint32_t rand_state = 54238147;

static inline uint32_t XorShift32()
{
	// Not thread-safe!!
	uint32_t x = rand_state;
	x ^= x << 13;
	x ^= x >> 17;
	x ^= x << 15;
	rand_state = x;
	return x;
}

//static float randf() { return rand() * kInvRandMax; }
static float randf() { return (XorShift32() & 0xFFFFFF) / 16777216.0f; }

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

static vec3 random_unit_dir()
{
	float z = randf() * 2 - 1;
	float a = randf() * 2 * float(M_PI);
	float r = sqrtf(1 - z * z);
	float x = r * cosf(a);
	float y = r * sinf(a);
	return vec3(x, y, z);
}

#endif
