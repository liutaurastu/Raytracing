//#include "stdafx.h"
#include "sphere.h"

Sphere::Sphere(float _radius, vec3 _center, vec3 _color, int _tag) {
	radius = _radius;
	center = _center;
	albedo = _color;
	tag = _tag;

	boundingBox.SetMin(vec3(center - radius));
	boundingBox.SetMax(vec3(center + radius));
}

IntersectionData Sphere::Intersection(const vec3 rayOrigin, const vec3 rayDirection, float & distance, vec3 &intersectionPoint)
{
	IntersectionData data;
	float t0, t1, tt; // solutions for t if the ray intersects 

	// geometric solution  // vector dir has to be normalized, length is 1.0
	vec3 L = center - rayOrigin;
	float tca = dot(L, rayDirection);

	if (tca < 0)
	{
		data.intersect = false;
		return data;
	}
	float d = dot(L, L) - tca * tca;
	if (d > (radius * radius))
	{
		data.intersect = false;
		return data;
	}
	float thc = sqrt(radius * radius - d);
	t0 = tca - thc;
	t1 = tca + thc;
	if (t0 > t1) 
	{
		tt = t1;
		t1 = t0;
		t0 = tt;
	}
	if (t0 < 0) 
	{
		t0 = t1; // if t0 is negative, let's use t1 instead 
		if (t0 < 0)
		{
			data.intersect = false;
			return data; // both t0 and t1 are negative 
		}
	}
	distance = t0;

	intersectionPoint = rayDirection * distance + rayOrigin;

	//normVec = normalize(IntPt - position);

	data.distance = distance;
	data.intersect = true;
	data.intersectionPoint = intersectionPoint;
	data.normal = normalize(distance * rayDirection - center); // + rayorigin to accunt for origin poisition. Already accounted for in plane and triangle formulas
	data.reflectivity = reflectivity;

	return data;
}

void Sphere::ComputeColor(const IntersectionData data, const vec3 rayOrigin, const vec3 rayDirection, const vec3 lightPosition, const vec3 lightColor, const float ambienceIntensity, vec3 &ColValue)
{
	vec3 lightRay, lightReflection, surfaceNormal, ambience, diffuse, specularity;

	// Calculate pixel ambience
	ambience = albedo * ambienceIntensity;

	// Calculate pixel diffuse
	lightRay = normalize(lightPosition - data.intersectionPoint);
	surfaceNormal = data.normal;
	diffuse = albedo * lightColor * (float)glm::max(0.0, (double)dot(lightRay, surfaceNormal));

	// Calculate pixel specularity
	lightReflection = 2 * dot(lightRay, surfaceNormal) * surfaceNormal - lightRay;
	specularity = specularColor * (float)pow(glm::max(0.0, (double)dot(lightReflection, -rayDirection)), shininess);

	ColValue = ambience + diffuse; // Only add specularity if diffuse is positive
	if (diffuse.x > 0 && diffuse.y > 0 && diffuse.z > 0) ColValue = ColValue + specularity;
}

Sphere::~Sphere()
{
}