#pragma once
#include "AxisAlignedBoundingBox.h"

#include <glm/glm.hpp>
#include <vector>
#include <string>
#include <iostream>

using namespace std;
using namespace glm;

struct IntersectionData
{
	vec3 intersectionPoint = vec3(0.0f, 0.0f, 0.0f);
	vec3 normal = vec3(0.0f, 0.0f, 0.0f);
	float distance = 0;
	float reflectivity;
	bool intersect = false;
};

class Shape
{
protected:
	vec3 albedo;
	// Default object attributes, change with shape->Set();
	vec3 specularColor = vec3(0.2f, 0.2f, 0.2f);
	float reflectivity = 0.2f;
	int shininess = 20;

	int tag = 0;
	AxisAlignedBoundingBox boundingBox;
	vector<Shape*> meshTriangles;

public:
	Shape();

	virtual IntersectionData Intersection(const vec3 rayOrigin, const vec3 rayDirection, float& t, vec3& intersectionPoint) = 0;
	virtual void ComputeColor(const IntersectionData data, const vec3 rayOrigin, const vec3 rayDirection, const vec3 lightPosition, const vec3 lightColor, const float ambienceIntensity, vec3& ColValue) = 0;

	// Setters
	void SetColor(vec3 color);
	void SetReflectivity(float _reflectivity);
	void SetShininess(int _shininess);
	void SetSpecularColor(vec3 _specularColor);

	// Getters
	vec3 GetColor();
	float GetReflectivity();
	int GetShininess();
	vec3 GetSpecularColor();
	int GetTag();
	AxisAlignedBoundingBox GetBoundingBox();
	vector<Shape*> GetMeshTriangles();

	~Shape();
};
