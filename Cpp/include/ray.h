#ifndef RAYH
#define RAYH

#include "vec3.h"

class ray
{
public:
    ray() {};
    ray( const vec3& a, const vec3& b) : A(a), B(b) {}
    vec3 origin() const { return A; }
	const vec3& origref() const { return A; }
    vec3 direction() const { return B; }
	const vec3& dirref() const { return B; }
	void set(const vec3& a, const vec3& b) { A = a; B = b; }
    vec3 point_at_parameter( float t ) const { return A + t * B; }

    vec3 A;
    vec3 B;
};

#endif
