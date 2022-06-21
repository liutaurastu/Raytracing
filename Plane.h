#pragma once
#include "Shape.h"
#include <glm/glm.hpp>

class Plane : public Shape
{
private:
	vec3 base;
	vec3 normal;

public:
	Plane(vec3 _base, vec3 _normal, vec3 _color, int _tag = 0);

	IntersectionData Intersection(const vec3 rayOrigin, const vec3 rayDirection, float& t, vec3& intersectionPoint) override;
	void ComputeColor(const IntersectionData data, const vec3 rayOrigin, const vec3 rayDirection, const vec3 lightPosition, const vec3 lightColor, const float ambienceIntensity, vec3 &ColValue) override;

	~Plane();
};

