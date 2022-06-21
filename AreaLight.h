#pragma once

#include <glm/glm.hpp>
#include <vector>

using namespace std;
using namespace glm;

class AreaLight
{
private:
	int sampleCount;
	vec3 position;
	vec3 size;
	vec3 lightColor;

	int lightsPerRow;
	int lightsPerColumn;

	float lightXSpacing;
	float lightZSpacing;

	float lightStartX;
	float lightStartY;
	float lightStartZ;

public:
	AreaLight(int _sampleCount, vec3 _position, vec3 _size, vec3 _lightColor);

	vector<vec3> CalculateSamplePoints();

	void UpdateSampleCount(int _sampleCount);

	//Setters
	void SetPosition(vec3 _position) { position = _position; UpdateSampleCount(sampleCount); }

	//Getters
	int GetSampleCount() { return sampleCount; };
	vec3 GetPosition() { return position; };
	vec3 GetSize() { return size; };
	vec3 GetColor() { return lightColor; };

	~AreaLight();
};

