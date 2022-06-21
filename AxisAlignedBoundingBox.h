#pragma once
#include "glm/glm.hpp"

using namespace glm;

class AxisAlignedBoundingBox
{
private:
	vec3 min;
	vec3 max;

public:
	AxisAlignedBoundingBox();

	bool CheckForIntersection(vec3 origin, vec3 rayDir); //check intersection with bouding box

	//Setters
	void SetMin(vec3 _min) { min = _min; }
	void SetMax(vec3 _max) { max = _max; }

	//Getters
	vec3 GetMin() { return min; }
	vec3 GetMax() { return max; }

	~AxisAlignedBoundingBox();
};