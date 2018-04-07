#include "sphere.h"

std::vector<sphere*> sphere::lights;

sphere::sphere(const vec3& c, float r, material* m) : hitable(), center(c), radius(r), mat(m)
{
	if (dot(m->emission, m->emission) > 0)
		lights.push_back(this);
}

bool sphere::hit(const ray& r, float t_min, float t_max, hit_record& rec) const
{
	// Note: some 2's have been simplified out
	vec3 oc = r.origin() - center;
	float a = dot(r.direction(), r.direction());
	float b = dot(oc, r.direction());
	float c = dot(oc, oc) - radius*radius;

	float delta = b*b - a*c;

	if (delta > 0)
	{
		float t = (-b - sqrt(delta)) / a;
		if (t < t_min || t > t_max)
		{
			t = (-b + sqrt(delta)) / a;
			if (t < t_min || t > t_max)
				return false;
		}

		rec.t = t;
		rec.P = r.point_at_parameter(t);
		rec.N = (rec.P - center) / radius;
		rec.id = id;
		rec.mat = mat;

		return true;
	}

	return false;
}