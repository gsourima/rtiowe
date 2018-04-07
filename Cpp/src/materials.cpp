#include "materials.h"

#include <cmath>
#include <algorithm> // std::max

#include "sphere.h"

//#define SAMPLE_LIGHTS

material* material::default = new lambertian(1);

bool lambertian::scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered, vec3& direct_light, unsigned int& rays_cast) const
{
	scattered = ray(rec.P, rec.N + random_in_unit_sphere());

	attenuation = albedo;

	if (checker)
	{
		if ((int(floorf(2 * rec.P.x()) + floorf(2 * rec.P.z())) % 2) == 0)
			attenuation *= 0.5f;
	}

#ifdef SAMPLE_LIGHTS
	for each (sphere* sphere_light in sphere::lights)
	{
		ray light_ray(rec.P, unit_vector(sphere_light->center + random_in_unit_sphere() * sphere_light->radius - rec.P));

		hit_record rec_light;

		rays_cast++;

		if (hitable::scene->hit(light_ray, 0.000001f, 100000.0f, rec_light))
		{
			if (rec_light.id == sphere_light->id)
			{
				vec3 scale = 0.25f * attenuation * float(M_1_PI) / sphere_light->radius;
				float dist = (sphere_light->center - rec.P).length();
				scale *= 1.0f / (1+dist*dist);
				direct_light = (scale * sphere_light->mat->emission) * std::max(0.0f, dot(unit_vector(light_ray.direction()), unit_vector(rec.N)));
			}
		}
	}
#endif

	return true;
}

bool metal::scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered, vec3& direct_light, unsigned int& rays_cast) const
{
	vec3 reflected = r_in.direction().reflect(rec.N) + fuzz*random_in_unit_sphere();

	scattered = ray(rec.P, unit_vector(reflected));

	attenuation = albedo;

	return (dot(scattered.direction(), rec.N) > 0);
}

bool dielectric::scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered, vec3& direct_light, unsigned int& rays_cast) const
{
	vec3 outward_normal;
	vec3 reflected = unit_vector(r_in.direction().reflect(rec.N));
	vec3 refracted;
	float ni_over_nt;
	float reflect_prob;
	float cosine;
	float din = dot(r_in.direction(), rec.N);
	attenuation = vec3(1);

	if (din > 0)
	{
		outward_normal = -rec.N;
		ni_over_nt = ridx;
		cosine = ridx * din / r_in.direction().length();
	}
	else
	{
		outward_normal = rec.N;
		ni_over_nt = 1 / ridx;
		cosine = -din / r_in.direction().length();
	}

	if (r_in.direction().refract(outward_normal, ni_over_nt, refracted))
	{
		reflect_prob = schlick(cosine, ridx);
	}
	else
	{
		reflect_prob = 1;
	}

	if (randf() < reflect_prob)
		scattered = ray(rec.P, reflected);
	else
		scattered = ray(rec.P, refracted);

	return true;
}