#include "Plane.h"

Plane::Plane(vec3 _base, vec3 _normal, vec3 _color, int _tag)
{
	base = _base;
	normal = _normal;
	albedo = _color;
	tag = _tag;

	boundingBox.SetMin(vec3(-100.0f, base.y - 10.0f, -100.0f));
	boundingBox.SetMax(vec3(100.0f, base.y, 100.0f));
}

IntersectionData Plane::Intersection(const vec3 rayOrigin, const vec3 rayDirection, float &distance, vec3 &intersectionPoint)
{
	IntersectionData data;

	distance = dot((base - rayOrigin), normal) / dot(rayDirection, normal);
	intersectionPoint = rayDirection * distance + rayOrigin;

	if (distance < 0)
	{
		data.intersect = false;
		return data;
	}

	data.distance = distance;
	data.intersect = true;
	data.intersectionPoint = intersectionPoint;
	data.normal = normal;
	data.reflectivity = reflectivity;

	return data;
}

void Plane::ComputeColor(const IntersectionData data, const vec3 rayOrigin, const vec3 rayDirection, const vec3 lightPosition, const vec3 lightColor, const float ambienceIntensity, vec3 &ColValue)
{
	vec3 lightRay, lightReflection, surfaceNormal, ambience, diffuse, specularity;

	// Calculate pixel ambience
	ambience = albedo * ambienceIntensity;

	// Calculate pixel diffuse
	lightRay = normalize(lightPosition - data.intersectionPoint);
	surfaceNormal = normal;
	diffuse = albedo * lightColor * (float)glm::max(0.0, (double)dot(lightRay, surfaceNormal));
	
	// Calculate pixel specularity
	lightReflection = 2 * dot(lightRay, surfaceNormal) * surfaceNormal - lightRay;
	specularity = specularColor * (float)pow(glm::max(0.0, (double)dot(lightReflection, -rayDirection)), shininess);

	ColValue = ambience + diffuse; // Only add specularity if diffuse is positive
	if (diffuse.x > 0 && diffuse.y > 0 && diffuse.z > 0) ColValue = ColValue + specularity;
}

Plane::~Plane()
{
}
