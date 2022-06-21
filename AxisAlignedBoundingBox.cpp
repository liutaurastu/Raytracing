#include "AxisAlignedBoundingBox.h"

AxisAlignedBoundingBox::AxisAlignedBoundingBox()
{
}

bool AxisAlignedBoundingBox::CheckForIntersection(vec3 origin, vec3 rayDir)
{
	float epsilon = 0.0001f, temp;

	float tmin = (min.x - origin.x) / (rayDir.x + epsilon);
	float tmax = (max.x - origin.x) / (rayDir.x + epsilon);

	if (tmin > tmax)
	{
		temp = tmin;
		tmin = tmax;
		tmax = temp;
	}

	float tymin = (min.y - origin.y) / (rayDir.y + epsilon);
	float tymax = (max.y - origin.y) / (rayDir.y + epsilon);

	if (tymin > tymax)
	{
		temp = tymin;
		tymin = tymax;
		tymax = temp;
	}

	if ((tmin > tymax) || (tymin > tmax))
		return false;

	if (tymin > tmin)
		tmin = tymin;

	if (tymax < tmax)
		tmax = tymax;

	float tzmin = (min.z - origin.z) / (rayDir.z + epsilon);
	float tzmax = (max.z - origin.z) / (rayDir.z + epsilon);

	if (tzmin > tzmax)
	{
		temp = tzmin;
		tzmin = tzmax;
		tzmax = temp;
	}

	if ((tmin > tzmax) || (tzmin > tmax))
		return false;

	return true;
}

AxisAlignedBoundingBox::~AxisAlignedBoundingBox()
{
}
