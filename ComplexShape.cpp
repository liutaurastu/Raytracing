#include "ComplexShape.h"

ComplexShape::ComplexShape(const char* _meshpath, vec3 _position,  vec3 _color, int _tag)
{
	position = _position;
	mesh = loadOBJ(_meshpath);

	// Create a triangle for every 3 verices of the mesh
	for (int i = 0; i < mesh.size(); i = i + 3)
	{
		Shape* tempTri = new Triangle(mesh[i].position + position, mesh[i + 1].position + position, mesh[i + 2].position + position, mesh[i].normal, mesh[i + 1].normal, mesh[i + 2].normal, _color, _tag);
		meshTriangles.push_back(tempTri);
	}
	tag = _tag;

	boundingBox.SetMin(mesh[0].position + position);
	boundingBox.SetMax(mesh[0].position + position);

	//Calculate min and max for bounding box
	for (int i = 0; i < mesh.size(); ++i)
	{
		if (boundingBox.GetMin().x > mesh[i].position.x + position.x) boundingBox.SetMin(vec3(mesh[i].position.x + position.x, boundingBox.GetMin().y, boundingBox.GetMin().z));
		if (boundingBox.GetMin().y > mesh[i].position.y + position.y) boundingBox.SetMin(vec3(boundingBox.GetMin().x, mesh[i].position.y + position.y, boundingBox.GetMin().z));
		if (boundingBox.GetMin().z > mesh[i].position.z + position.z) boundingBox.SetMin(vec3(boundingBox.GetMin().x, boundingBox.GetMin().y, mesh[i].position.z + position.z));

		if (boundingBox.GetMax().x < mesh[i].position.x + position.x) boundingBox.SetMax(vec3(mesh[i].position.x + position.x, boundingBox.GetMax().y, boundingBox.GetMax().z));
		if (boundingBox.GetMax().y < mesh[i].position.y + position.y) boundingBox.SetMax(vec3(boundingBox.GetMax().x, mesh[i].position.y + position.y, boundingBox.GetMax().z));
		if (boundingBox.GetMax().z < mesh[i].position.z + position.z) boundingBox.SetMax(vec3(boundingBox.GetMax().x, boundingBox.GetMax().y, mesh[i].position.z + position.z));
	}
}

IntersectionData ComplexShape::Intersection(const vec3 rayOrigin, const vec3 rayDirection, float& distance, vec3& intersectionPoint)
{
	IntersectionData data;
	return data;
}

void ComplexShape::ComputeColor(const IntersectionData data, const vec3 rayOrigin, const vec3 rayDirection, const vec3 lightPosition, const vec3 lightColor, const float ambienceIntensity, vec3& ColValue)
{
}

ComplexShape::~ComplexShape()
{
}
