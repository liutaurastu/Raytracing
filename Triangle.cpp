#include "Triangle.h"

Triangle::Triangle(vec3 v0, vec3 v1, vec3 v2, vec3 n0, vec3 n1, vec3 n2, vec3 _color, int _tag)
{
	vertex0 = v0;
	vertex1 = v1;
	vertex2 = v2;
	normal0 = n0;
	normal1 = n1;
	normal2 = n2;
	albedo = _color;
	tag = _tag;

	boundingBox.SetMin(min(v0, min(v1, v2)));
	boundingBox.SetMax(max(v0, max(v1, v2)));
}

IntersectionData Triangle::Intersection(const vec3 rayOrigin, const vec3 rayDirection, float& distance, vec3& intersectionPoint)
{
	IntersectionData data;

	vec3 edge1, edge2, edge3;
	float u, v, w;

	//Moller-Trumbore

	edge1 = vertex1 - vertex0;
	edge2 = vertex2 - vertex0;
	edge3 = vertex2 - vertex1;

	u = dot(rayOrigin - vertex0, cross(rayDirection, edge2)) / dot(edge1, cross(rayDirection, edge2));
	v = dot(rayDirection, cross(rayOrigin - vertex0, edge1)) / dot(edge1, cross(rayDirection, edge2));
	w = 1.0f - u - v;

	if (u < 0.0f || u > 1.0f)
	{
		data.intersect = false;
		return data;
	}
	if (v < 0.0f || u + v > 1.0f)
	{
		data.intersect = false;
		return data;
	}

	// Compute distance(t) and intersection point
	distance = dot(edge2, cross(rayOrigin - vertex0, edge1)) / dot(edge1, cross(rayDirection, edge2));

	intersectionPoint = distance * rayDirection + rayOrigin;
	//intersectionPoint = w * normal0 + u * normal1 + v * normal2;

	data.distance = distance;
	data.intersect = true;
	data.intersectionPoint = intersectionPoint;
	data.normal = normalize(w * normal0 + u * normal1 + v * normal2); // I found that this gives the complex shapes's edges a smoothed look
	//data.normal = normalize(cross(edge1, edge2));
	data.reflectivity = reflectivity;

	return data;
}

void Triangle::ComputeColor(const IntersectionData data, const vec3 rayOrigin, const vec3 rayDirection, const vec3 lightPosition, const vec3 lightColor, const float ambienceIntensity, vec3 &ColValue)
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

	ColValue = ambience + diffuse + specularity;
}

Triangle::~Triangle()
{
}
