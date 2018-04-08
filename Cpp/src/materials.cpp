#include "materials.h"

#include <cmath>
#include <algorithm> // std::max

#include "sphere.h"

#define SAMPLE_LIGHTS

material* material::default = new lambertian(1);

bool lambertian::scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered, vec3& direct_light, unsigned int& rays_cast) const
{
	scattered = ray(rec.P, rec.N + random_unit_dir());

	attenuation = albedo;

	if (checker)
	{
		if ((int(floorf(2 * rec.P.x()) + floorf(2 * rec.P.z())) % 2) == 0)
			attenuation *= 0.5f;
	}

#ifdef SAMPLE_LIGHTS
	for each (sphere* sphere_light in sphere::lights)
	{
		float r = sphere_light->radius;

		// Naive GS version (equivalent to Aras' method)
		ray light_ray(rec.P, unit_vector(sphere_light->center + random_unit_dir() * r - rec.P));

		// Aras sampling method
        // create a random direction towards sphere
        // coord system for sampling: sw, su, sv
		//vec3 sw = unit_vector(sphere_light->center - rec.P);
		//vec3 su = unit_vector(cross(fabs(sw.x())>0.01f ? vec3(0, 1, 0) : vec3(1, 0, 0), sw));
		//vec3 sv = cross(sw, su);
		//// sample sphere by solid angle
		//float cosAMax = sqrtf(1.0f - r*r / (rec.P - sphere_light->center).squared_length());
		//float eps1 = randf(), eps2 = randf();
		//float cosA = 1.0f - eps1 + eps1 * cosAMax;
		//float sinA = sqrtf(1.0f - cosA*cosA);
		//float phi = 2 * float(M_PI) * eps2;
		//vec3 l = su * cosf(phi) * sinA + sv * sin(phi) * sinA + sw * cosA;
		//l.make_unit_vector();
		//ray light_ray(rec.P, l);

		hit_record rec_light;

		#pragma omp atomic	
		rays_cast++;

		if (hitable::scene->hit(light_ray, 0.001f, 10000.0f, rec_light))
		{
			if (rec_light.id == sphere_light->id)
			{
				// Naive GS version
				// float dist2 = (rec_light.P - rec.P).squared_length();
				float dist2 = (sphere_light->center - rec.P).squared_length();
				vec3 scale = attenuation / ( 4 * float(M_PI) * r*r );
				scale *= std::max(0.0f, dot(-light_ray.direction(), unit_vector(rec_light.N)));
				scale *= std::min(1.0f, 1.0f / dist2 );

				// Aras version (solid angle)
				//vec3 scale = attenuation * ( 2 - 2 * cosAMax );

				direct_light += (scale * sphere_light->mat->emission) * std::max(0.0f, dot(light_ray.direction(), unit_vector(rec.N)));
			}
		}
	}
#endif

	return true;
}

bool metal::scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered, vec3& direct_light, unsigned int& rays_cast) const
{
	vec3 reflected = r_in.direction().reflect(rec.N) + fuzz*random_unit_dir();

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