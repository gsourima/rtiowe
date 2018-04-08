#include "sphere.h"

std::vector<sphere*> sphere::lights;

sphere::sphere(const vec3& c, float r, material* m) : hitable(), center(c), radius(r), mat(m)
{
	inv_radius = 1.0f / r;

	if (dot(m->emission, m->emission) > 0)
		lights.push_back(this);
}

bool sphere::hit(const ray& r, float t_min, float t_max, hit_record& rec) const
{
	// Note: some 2's have been simplified out
	vec3 oc = r.origref() - center;
	const vec3& rdir = r.dirref();
	float a = dot(rdir, rdir);
	float b = dot(oc, rdir);
	float c = dot(oc, oc) - radius*radius;

	float delta = b*b - a*c;

	if (delta > 0)
	{
		float inv_a = 1.0f / a;
		float b_over_a = b * inv_a;
		float sqrt_d_over_a = sqrtf(delta) * inv_a;

		float t = - b_over_a - sqrt_d_over_a;
		if (t < t_min || t > t_max)
		{
			t = -b_over_a + sqrt_d_over_a;
			if (t < t_min || t > t_max)
				return false;
		}

		rec.t = t;
		rec.P = r.point_at_parameter(t);
		rec.N = (rec.P - center) * inv_radius;
		rec.id = id;
		rec.mat = mat;

		return true;
	}

	return false;
}