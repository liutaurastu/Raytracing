#pragma once
#include "Shape.h"
#include "Triangle.h"
#include "OBJLoader.h"
#include <glm/glm.hpp>

using namespace std;

class ComplexShape : public Shape
{
private:
	vector<VertexWithAll> mesh;
	vec3 position;

public:
	ComplexShape(const char* _meshpath, vec3 position,  vec3 _color, int _tag = 0);

	IntersectionData Intersection(const vec3 rayOrigin, const vec3 rayDirection, float& t, vec3& intersectionPoint) override;
	void ComputeColor(const IntersectionData data, const vec3 rayOrigin, const vec3 rayDirection, const vec3 lightPosition, const vec3 lightColor, const float ambienceIntensity, vec3& ColValue) override;

	~ComplexShape();
};