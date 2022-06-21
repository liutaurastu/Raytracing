#pragma once
#include "AreaLight.h"
#include "Shape.h"

#include <iostream>
#include <sdl/SDL.h>
#include <thread>
#include <vector>

class Engine
{
private:
	const int SCREEN_WIDTH = 800;
	const int SCREEN_HEIGHT = 600;
	const int threadCount = 32;
	const vec3 backgroundColor = vec3(1.0f, 1.0f, 1.0f);

	// Default changable values
	vec3 lightPosition = vec3(1.0f, 10.0f, 13.0f);
	float fov = 30.0f;
	float ambientIntensity = 0.1f;
	int lightSampleCount = 36; // preferably 0(no shadows), 1(hard shadows) or the first 12 perfect square numbers (until 169).
	int reflectionCount = 2; // Requires adjustment if set higher

	// Vars for engine update logic
	int threadsFinished = threadCount;
	bool stopThreads = true;

	SDL_Window* window = NULL;
	SDL_Surface* screenSurface = NULL;

	// Camera and pixel calculation vars
	vec3** image;
	vec3 rayOrigin = vec3(0.0f, 0.0f, 0.0f);
	float Iar;
	float tanvalue; 

	// Setup window, screen surface, renderer and texture
	bool InitSDL(SDL_Window*& window, SDL_Surface*& screenSurface);

	// Screen calculations
	void CalculateSurfaceColor(SDL_Surface* surface, AreaLight areaLight, vector<Shape*> shapes, int start, int end);
	void IntersectAndGetColor(AreaLight &areaLight, vector<Shape*> shapes, vec3 rayOrigin, vec3 rayDirection, vec3 &pointColor);
	void ReflectionRay(AreaLight areaLight, vector<Shape*> shapes, vec3 rayOrigin, vec3 rayDirection, vec3& pointColor, int depth);
	void RecalculateTanValue();

	// Color Conversions
	void PutPixel32_nolock(SDL_Surface*& surface, int x, int y, Uint32 colour);
	Uint32 ConvertColour(vec3 colour);

public:

	Engine();
	
	void StartUpdate(AreaLight areaLight, vector<Shape*> shapes);
	void CloseEngine(vector<Shape*> shapes);

	// Getters
	int GetLightSampleCount();
	vec3 GetLightPosition();

	~Engine();
};

