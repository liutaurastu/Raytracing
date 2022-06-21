#pragma once
#include "Shape.h"
#include <glm/glm.hpp>

class Sphere : public Shape
{
private:
	float radius;
	vec3 center;

public:
	Sphere(float _radius, vec3 _center, vec3 _color, int _tag = 0);

	IntersectionData Intersection(const vec3 rayOrigin, const vec3 rayDirection, float& t, vec3& intersectionPoint) override;
	void ComputeColor(const IntersectionData data, const vec3 rayOrigin, const vec3 rayDirection, const vec3 lightPosition, const vec3 lightColor, const float ambienceIntensity, vec3 &ColValue) override;

	~Sphere();
};