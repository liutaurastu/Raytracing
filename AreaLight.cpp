#include "AreaLight.h"

AreaLight::AreaLight(int _sampleCount, vec3 _position, vec3 _size, vec3 _lightColor)
{
	sampleCount = _sampleCount;
	position = _position;
	size = _size;
	lightColor = _lightColor;

	// Do as much calculations on start as possible - every bit saves load time, especially with square root
	lightsPerRow = sqrt(sampleCount);
	lightsPerColumn = sqrt(sampleCount);

	// If there is only one lightsource, use hard shadows
	if (sampleCount == 1)
	{
		lightXSpacing = 0;
		lightZSpacing = 0;
	}
	else
	{
		lightXSpacing = size.x / lightsPerRow;
		lightZSpacing = size.z / lightsPerColumn;
	}

	lightStartX = position.x - size.x * 0.5f;
	lightStartY = position.y - size.y * 0.5f;
	lightStartZ = position.z - size.z * 0.5f;
}

vector<vec3> AreaLight::CalculateSamplePoints()
{
	//float sizeRatio = size.x / size.z; // 16:9  x = 4, z = 3 ; 9:9 x = 3, z = 3 ; 4:25 x = 2, z = 5;

	vector<vec3> lightPoints;

	float xJitterAmount;
	float zJitterAmount;

	// Array for calculating all light source positions
	for (int i = 0; i < lightsPerRow; ++i)
	{
		for (int j = 0; j < lightsPerColumn; ++j)
		{
			// Thanks to https://stackoverflow.com/questions/13408990/how-to-generate-random-float-number-in-c
			xJitterAmount = ((float)rand() / (float)(RAND_MAX)) * lightXSpacing;
			zJitterAmount = ((float)rand() / (float)(RAND_MAX)) * lightZSpacing;

			lightPoints.push_back(vec3(lightStartX + lightXSpacing * i + xJitterAmount, lightStartY,
				lightStartZ + lightZSpacing * j + zJitterAmount));
		}
	}

	return lightPoints;
}

void AreaLight::UpdateSampleCount(int _sampleCount)
{
	sampleCount = _sampleCount;

	lightsPerRow = sqrt(sampleCount);
	lightsPerColumn = sqrt(sampleCount);

	// If there is only one lightsource, use hard shadows
	if (sampleCount == 1)
	{
		lightXSpacing = 0;
		lightZSpacing = 0;
	}
	else
	{
		lightXSpacing = size.x / lightsPerRow;
		lightZSpacing = size.z / lightsPerColumn;
	}

	lightStartX = position.x - size.x * 0.5f;
	lightStartY = position.y - size.y * 0.5f;
	lightStartZ = position.z - size.z * 0.5f;
}

AreaLight::~AreaLight()
{
}
