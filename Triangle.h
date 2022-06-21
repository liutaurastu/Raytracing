#pragma once
#include "Shape.h"
#include <glm/glm.hpp>

class Triangle : public Shape
{
protected:
    vec3 vertex0, vertex1, vertex2;
    vec3 normal0, normal1, normal2;

public:
    Triangle(vec3 v0, vec3 v1, vec3 v2, vec3 n0, vec3 n1, vec3 n2, vec3 color, int _tag = 0);

    IntersectionData Intersection(const vec3 rayOrigin, const vec3 rayDirection, float& t, vec3& intersectionPoint) override;
    void ComputeColor(const IntersectionData data, const vec3 rayOrigin, const vec3 rayDirection, const vec3 lightPosition, const vec3 lightColor, const float ambienceIntensity, vec3 &ColValue) override;

    ~Triangle();
};