#include "Shape.h"

Shape::Shape()
{
}

inline void Shape::SetColor(vec3 color)
{
	albedo = color;
}

void Shape::SetReflectivity(float _reflectivity)
{
	reflectivity = _reflectivity;
}

void Shape::SetShininess(int _shininess)
{
	shininess = _shininess;
}

void Shape::SetSpecularColor(vec3 _specularColor)
{
	specularColor = _specularColor;
}

inline vec3 Shape::GetColor()
{
	return albedo;
}

float Shape::GetReflectivity()
{
	return reflectivity;
}

int Shape::GetShininess()
{
	return shininess;
}

vec3 Shape::GetSpecularColor()
{
	return specularColor;
}

int Shape::GetTag()
{
	return tag;
}

AxisAlignedBoundingBox Shape::GetBoundingBox()
{
	return boundingBox;
}

vector<Shape*> Shape::GetMeshTriangles()
{
	return meshTriangles;
}

Shape::~Shape()
{
	// Delete all mesh triangles
	for (const auto& triangle : meshTriangles)
	{
		delete triangle;
	}
}
