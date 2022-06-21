#include "Engine.h"
#include <GL/glew.h>

Engine::Engine()
{
	InitSDL(window, screenSurface);
}

bool Engine::InitSDL(SDL_Window* &window, SDL_Surface* &screenSurface)
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		cout << "SDL could not initialize! SDL_Error: " << SDL_GetError() << endl;
		return false;
	}
	else
	{
		//create the window
		window = SDL_CreateWindow("Raytracing", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		if (window == NULL)
		{
			cout << "SDL Window could not be created! SDL_Error: " << SDL_GetError() << endl;
			return false;
		}
		else
		{
			screenSurface = SDL_GetWindowSurface(window);
		}
	}

	// Causes memory leaks if left in CalculateSurfaceColor
	// Create two dimensional pxiel array for the image
	image = new vec3 * [SCREEN_WIDTH];
	for (int i = 0; i < SCREEN_WIDTH; ++i) image[i] = new vec3[SCREEN_HEIGHT];

	Iar = SCREEN_WIDTH / (float)SCREEN_HEIGHT;
	RecalculateTanValue();
	
	return true;
}

void Engine::StartUpdate(AreaLight areaLight, vector<Shape*> shapes)
{
	vector<std::thread> threads; // Create array of threads
	int threadInaccuracy = 0;
	int softLockCycles = 0;

	int lightSampleIndex = 0;
	vector<int> lightSampleStates = { 0, 1, 4, 9, 25, 36, 49, 64, 81, 100, 144, 169};

	// To make it less hardcodey
	for (int i = 0; i < lightSampleStates.size(); ++i)
	{
		if(lightSampleStates[i] == lightSampleCount) lightSampleIndex = lightSampleCount;
	}

	// Add threads
	for (int i = 0; i < threadCount; ++i) threads.push_back(thread());

	// Performance measurements
	Uint64 start, end;
	float elapsedMS, totalMS = 0.0f;
	int frames = 0;
	bool startRecording = true;

	bool quit = false;

	//Defaults
	vec3 deflightPos = areaLight.GetPosition();
	float defFOV = fov;
	float defAmbient = ambientIntensity;
	int defReflectionCount = reflectionCount;
	int defLightSampleCount = lightSampleCount;

	vector<float> defReflectivities; // Get all objects default reflectivity
	for (Shape*& shape : shapes)
	{
		defReflectivities.push_back(shape->GetReflectivity());
	}
	
	vector<int> defShinenesses; // Get all objects default shininesses
	for (Shape*& shape : shapes)
	{
		defShinenesses.push_back(shape->GetShininess());
	}
	cout << "-------------------------------------\n";
	cout << "Q, A - Change ambient intensity\n";
	cout << "W, S - Change light sample count\n";
	cout << "E, D - Change reflection count\n";
	cout << "R, F - Change all object reflectivity\n";
	cout << "T, G - Change all object shininess\n";
	cout << "Z, X - Change FOV (Zoom)\n";
	cout << "Arrow keys - Change light position\n";
	cout << "Space - Reset to defaults\n";
	cout << "-------------------------------------\n";

	// Real update starts here
	while (!quit)
	{
		if (startRecording) // Enable performance counting
		{
			start = SDL_GetPerformanceCounter();
			startRecording = false;
		}
		SDL_Event event;

		//Keyboard Input. Keep this closed when working on something else
		while (SDL_PollEvent(&event) != 0)
		{
			if (event.type == SDL_QUIT)
			{
				stopThreads = true;
				if (threads.size() > 0) for (auto& thread : threads) if(thread.joinable()) thread.detach(); // Join all threads (in other words wait for threads to finish their work so we can start more)
				quit = true;
			}
			if (event.type == SDL_KEYDOWN && event.key.repeat == 0)
			{
				// Stop calculating pixels when any input is given
				stopThreads = true;

				// keyboard API for key pressed
				switch (event.key.keysym.scancode) {
				case SDL_SCANCODE_ESCAPE:
				{
					stopThreads = true;
					if (threads.size() > 0) for (auto& thread : threads) if (thread.joinable()) thread.detach(); // Join all threads (in other words wait for threads to finish their work so we can start more)
					quit = true;
				}
				case SDL_SCANCODE_Q:
				{
					cout << "Ambient Intensity: " << ambientIntensity << " -> ";
					ambientIntensity = ambientIntensity + 0.05f;
					cout << ambientIntensity << endl;
					break;
				}
				case SDL_SCANCODE_A:
				{
					cout << "Ambient Intensity: " << ambientIntensity << " -> ";
					ambientIntensity = ambientIntensity - 0.05f;
					cout << ambientIntensity << endl;
					break;
				}
				case SDL_SCANCODE_W:
				{
					lightSampleIndex++;
					if (lightSampleIndex > lightSampleStates.size() - 1) lightSampleIndex = 0;

					cout << "Light Sample Size: " << lightSampleCount << " -> ";
					lightSampleCount = lightSampleStates[lightSampleIndex];
					areaLight.UpdateSampleCount(lightSampleCount);
					cout << lightSampleCount << endl;
					break;
				}
				case SDL_SCANCODE_S:
				{
					lightSampleIndex--;
					if (lightSampleIndex < 0) lightSampleIndex = lightSampleStates.size() - 1;

					cout << "Light Sample Size: " << lightSampleCount << " -> ";
					lightSampleCount = lightSampleStates[lightSampleIndex];
					areaLight.UpdateSampleCount(lightSampleCount);
					cout << lightSampleCount << endl;
					break;
				}
				case SDL_SCANCODE_E:
				{
					if (reflectionCount < 8)
					{
						cout << "Reflections: " << reflectionCount << " -> ";
						reflectionCount = reflectionCount + 1;
						cout << reflectionCount << endl;
					}
					else cout << "Reflection count cannot be any higher!" << endl;
					break;
				}
				case SDL_SCANCODE_D:
				{
					if (reflectionCount > 0)
					{
						cout << "Reflections: " << reflectionCount << " -> ";
						reflectionCount = reflectionCount - 1;
						cout << reflectionCount << endl;
					}
					else cout << "Reflection count cannot be any lower!" << endl;
					break;
				}
				case SDL_SCANCODE_R:
				{
					for (Shape*& shape : shapes)
					{
						if (shape->GetReflectivity() + 0.05f <= 10.0f)
						{
							cout << "Shape reflectivity: ";
							shape->SetReflectivity(shape->GetReflectivity() + 0.05f);
							cout << shape->GetReflectivity() - 0.05f << " -> " << shape->GetReflectivity() << endl;
						}
						else
						{
							cout << "Object reflectivity cannot go any higher!\n";
							break;
						}
					}
					break;
				}
				case SDL_SCANCODE_F:
				{
					for (Shape*& shape : shapes)
					{
						if (shape->GetReflectivity() - 0.05f > 0)
						{
							cout << "Shape reflectivity: ";
							shape->SetReflectivity(shape->GetReflectivity() - 0.05f);
							cout << shape->GetReflectivity() + 0.05f << " -> " << shape->GetReflectivity() << endl;
						}
						else
						{
							cout << "Object reflectivity cannot go any lower!\n";
							break;
						}
					}
					break;
				}
				case SDL_SCANCODE_T:
				{
					for (Shape*& shape : shapes)
					{
						if (shape->GetShininess() + 5 <= 100)
						{
							cout << "Shape shiniess: ";
							shape->SetShininess(shape->GetShininess() + 5);
							cout << shape->GetShininess() - 5 << " -> " << shape->GetShininess() << endl;
						}
						else
						{
							cout << "Object shininess cannot go any higher!\n";
							break;
						}
					}
					break;
				}
				case SDL_SCANCODE_G:
				{
					for (Shape*& shape : shapes)
					{
						if (shape->GetShininess() - 5 > 0)
						{
							cout << "Shape shiniess: ";
							shape->SetShininess(shape->GetShininess() - 5);
							cout << shape->GetShininess() + 5 << " -> " << shape->GetShininess() << endl;
						}
						else 
						{
							cout << "Object shininess cannot go any lower!\n";
							break;
						}
					}
					break;
				}
				case SDL_SCANCODE_Z:
				{
					if (fov > 10.0f)
					{
						cout << "FOV: " << fov << " -> ";
						fov = fov - 2.5f;
						cout << fov << endl;
						RecalculateTanValue();
					}
					else cout << "FOV cannot be any lower!" << endl;
					break;
				}
				case SDL_SCANCODE_X:
				{
					if (fov < 57.5f)
					{
						cout << "FOV: " << fov << " -> ";
						fov = fov + 2.5f;
						cout << fov << endl;
						RecalculateTanValue();
					}
					else cout << "FOV cannot be any higher!" << endl;
					break;
				}
				case SDL_SCANCODE_UP:
				{
					cout << "Light position Z: " << areaLight.GetPosition().z << " -> ";
					areaLight.SetPosition(vec3(areaLight.GetPosition().x, areaLight.GetPosition().y, areaLight.GetPosition().z - 6.0f));
					cout << areaLight.GetPosition().z << endl;
					break;
				}
				case SDL_SCANCODE_DOWN:
				{
					cout << "Light position Z: " << areaLight.GetPosition().z << " -> ";
					areaLight.SetPosition(vec3(areaLight.GetPosition().x, areaLight.GetPosition().y, areaLight.GetPosition().z + 6.0f));
					cout << areaLight.GetPosition().z << endl;
					break;
				}
				case SDL_SCANCODE_LEFT:
				{
					cout << "Light position X: " << areaLight.GetPosition().x << " -> ";
					areaLight.SetPosition(vec3(areaLight.GetPosition().x - 6.0f, areaLight.GetPosition().y, areaLight.GetPosition().z));
					cout << areaLight.GetPosition().x << endl;
					break;
				}
				case SDL_SCANCODE_RIGHT:
				{
					cout << "Light position X: " << areaLight.GetPosition().x << " -> ";
					areaLight.SetPosition(vec3(areaLight.GetPosition().x + 6.0f, areaLight.GetPosition().y, areaLight.GetPosition().z));
					cout << areaLight.GetPosition().x << endl;
					break;
				}
				case SDL_SCANCODE_SPACE:
				{
					cout << "Reset to defaults.\n";

					// Set to default light position, reflection count and ambient intensity
					areaLight.SetPosition(deflightPos);
					reflectionCount = defReflectionCount;
					ambientIntensity = defAmbient;

					// Set default FOV
					fov = defFOV;
					RecalculateTanValue();

					// Set default light point count
					lightSampleCount = defLightSampleCount; 
					areaLight.UpdateSampleCount(lightSampleCount);

					// Set each objects refault reflectivity
					for (int i = 0; i < shapes.size(); ++i)
					{
						shapes[i]->SetReflectivity(defReflectivities[i]);
					}

					// Set each objects refault shininess
					for (int i = 0; i < shapes.size(); ++i)
					{
						shapes[i]->SetShininess(defShinenesses[i]);
					}

					break;
				}
				default:
					break;
				}
			}
		}
	
		// Not good, threads can fall out of thread function without triggering threadsFinished interger (accounted for with a workaround)
		if (threadsFinished >= threadCount - threadInaccuracy && quit == false) // If all threads did their jobs
		{
			end = SDL_GetPerformanceCounter(); // Stop recording ticks
			startRecording = true; // Enable counting again next iteration

			if (stopThreads == false) // If we got here by keyboard input, then current frame performance data is redundant
			{
				// Print and calculate frame time and performance data
				frames++;
				elapsedMS = (end - start) / (float)SDL_GetPerformanceFrequency() * 1000.0f;
				totalMS = totalMS + elapsedMS;
				cout << "Frametime: " << elapsedMS << /*"\tFrame count: " << frames <<*/ " \tAverage frametime: " << totalMS / frames << " (" << frames << ")" << endl;
			}

			////////// Start Calculating Next Frame ///////////

			// Adjust for next frame
			stopThreads = false;
			threadsFinished = 0;
			threadInaccuracy = 0;

			// Call join before next operation to see the pixels updating in real time
			if (threads.size() > 0) for (auto& thread : threads) if(thread.joinable()) thread.detach(); // Join all threads (in other words wait for threads to finish their work so we can start more)

			for (int i = 0; i < threadCount; ++i) // Update each thread surface area (because pushing back each time could be expensive)
			{
				threads[i] = (std::thread(&Engine::CalculateSurfaceColor, this, screenSurface, areaLight, shapes, i * SCREEN_WIDTH / threadCount, (i + 1) * SCREEN_WIDTH / threadCount));
			}
		}
		else
		{
			// Softlock workaround (thread completion sometimes doesnt register which freezes the window)
			// Downsides: 
			// - sometimes on low settings micro freezes;
			// - after changing settings, first frame time can be lower than it should.
			if (threadsFinished > threadCount * 0.8f && threadsFinished <= threadCount - 1)
			{
				softLockCycles++;
			}
			else {
				softLockCycles = 0;
				
			}
			// if after 26 cycles threads finished is the same, means its a softlock, increase thread inaccuracy,
			// lower if you wish to lower the microstutters, but decrease frametime accuraccy.
			if (softLockCycles > 36) 
			{
				softLockCycles = 0;
				threadInaccuracy++;
			}
		}
		// Update screen
		SDL_UpdateWindowSurface(window);

		// Don't use too much processing power on Update while loop if it comes to that
		SDL_Delay(17);
	}
}

void Engine::CloseEngine(vector<Shape*> shapes)
{
	// Delete all shapes which were initialized with "new"
	int i = 0;
	for (auto& shape : shapes)
	{
		delete shape;
		i++;
	}
	cout << "Removed " << i << " shapes." << endl;

	SDL_DestroyWindow(window);
	SDL_Quit();
}

void Engine::CalculateSurfaceColor(SDL_Surface* surface, AreaLight areaLight, vector<Shape*> shapes, int start, int end)
{
	float PixelNdx, PixelNdy, PixelRdx, PixelRdy, PCameraX, PCameraY;

	vec3 pointColor;
	vec3 rayVector;

	//loop through pixels
	for (int x = start; x < end; ++x)
	{
		for (int y = 0; y < SCREEN_HEIGHT; ++y)
		{
			// If I want to stop threads in operation, check for bool, record another thread finished and return
			if (stopThreads == true) 
			{
				threadsFinished++;
				return;
			}

			//// To maybe see the pixel which is being caluclated
			//image[x][y] = vec3(0.9f, 0.3f, 0.0f);
			//PutPixel32_nolock(screenSurface, x, y, ConvertColour(image[x][y]));

			PixelNdx = (x + 0.5f) / (float)SCREEN_WIDTH;
			PixelNdy = (y + 0.5f) / (float)SCREEN_HEIGHT;
			PixelRdx = 2 * PixelNdx - 1.0f;
			PixelRdy = 1.0f - 2.0f * PixelNdy;
			PixelRdx = PixelRdx * Iar;
			PCameraX = PixelRdx * tanvalue;
			PCameraY = PixelRdy * tanvalue;

			rayVector.x = PCameraX;
			rayVector.y = PCameraY;
			rayVector.z = -1.0f;

			vec3 rayDirection = normalize(rayVector);

			IntersectAndGetColor(areaLight, shapes, rayOrigin, rayDirection, pointColor); // +5 Seconds
			if (reflectionCount > 0) ReflectionRay(areaLight, shapes, rayOrigin, rayDirection, pointColor, 0);	// +3.7 Seconds
			image[x][y] = pointColor;

			PutPixel32_nolock(screenSurface, x, y, ConvertColour(image[x][y]));
		}
	}
	threadsFinished++; // When thread finishes its function, record another finished thread
}

void Engine::IntersectAndGetColor(AreaLight &areaLight, vector<Shape*> shapes, vec3 rayOrigin, vec3 rayDirection, vec3& pointColor)
{
	IntersectionData data;

	vec3 lightRay, intersectionPoint, secondObjIntersectionPoint;

	float minimumDistance, distance, secondObjDistance;
	int closestIndex;

	vector<float> distances;
	vector<vec3> colors;
	vector<vec3> intersections;
	vector<IntersectionData> datas;
	vector<int> intersectingShapeTags;

	// Calculate Base Color
	for (const auto& shape : shapes)
	{
		// Check if we hit the objects bounding box. Culls the amount of interations which leads to much better performance
		if (shape->GetBoundingBox().CheckForIntersection(rayOrigin, rayDirection))
		{
			// The reason for this two part intersection checking logic is that it is much faster 
			// than doing complex shape logic inside their Intersection function.

			if (shape->GetTag() >= 1000) // Complex Shape
			{
				for (const auto& triangle : shape->GetMeshTriangles())
				{
					data = triangle->Intersection(rayOrigin, rayDirection, distance, intersectionPoint);
					if (data.intersect) // If we hit the object, calculate the color of that point
					{
						triangle->ComputeColor(data, rayOrigin, rayDirection, areaLight.GetPosition(), areaLight.GetColor(), ambientIntensity, pointColor);
						// Save data for later use
						distances.push_back(distance);
						colors.push_back(pointColor);
						intersections.push_back(intersectionPoint);
						datas.push_back(data);
						intersectingShapeTags.push_back(shape->GetTag());
					}
				}
			}
			else // Basic Shape
			{
				data = shape->Intersection(rayOrigin, rayDirection, distance, intersectionPoint);
				if (data.intersect) // If we hit the object, calculate the color of that point
				{
					shape->ComputeColor(data, rayOrigin, rayDirection, areaLight.GetPosition(), areaLight.GetColor(), ambientIntensity, pointColor);
					// Save data for later use
					distances.push_back(distance);
					colors.push_back(pointColor);
					intersections.push_back(intersectionPoint);
					datas.push_back(data);
					intersectingShapeTags.push_back(shape->GetTag());
				}
			}
		}
	}
	// To check how much memory intetrsection data occupies
	/*cout <<  "Size of Intersection data struct: "<< sizeof(datas[0]) << endl;
	cout << "Size of Intersection data intersection point: " << sizeof(datas[0].intersectionPoint) << endl;
	cout << "Size of Intersection data normal: " << sizeof(datas[0].normal) << endl;
	cout << "Size of Intersection data distance: " << sizeof(datas[0].distance) << endl;
	cout << "Size of Intersection data refelctivity: " << sizeof(datas[0].reflectivity) << endl;
	cout << "Size of Intersection data intersect: " << sizeof(datas[0].intersect) << endl;*/

	if (distances.size() == 0) pointColor = backgroundColor; // Assign background color if we hit nothing else
	else
	{
		// Out of all intersecting points on the pixel, find the nearest one
		minimumDistance = 1000.0f;
		closestIndex = 0;
		for (int i = 0; i < distances.size(); ++i)
		{
			if (distances[i] < minimumDistance)
			{
				closestIndex = i;
				minimumDistance = distances[i];
			}
		}
		intersectionPoint = intersections[closestIndex]; //+ datas[closestIndex].normal * 0.0001f; // Remove self shadowing effect, may change if I implement object checking

		for (const auto& lightPoint : areaLight.CalculateSamplePoints()) // Loop through each point of light
		{
			lightRay = normalize(lightPoint - intersectionPoint); // Get direction towards one of the light sources

			for (const auto& shape2 : shapes) // Iterate over every shape to see if any shape is covering lightsource
			{
				//Check if it is the same object and if we even CAN intersect by checking for object bounding box. MASSIVELY saves loading time
				if (shape2->GetBoundingBox().CheckForIntersection(intersectionPoint, lightRay) && shape2->GetTag() != intersectingShapeTags[closestIndex])
				{
					// The reason for this two part intersection checking logic is that it is much faster 
					// than doing complex shape logic inside their Intersection function.

					if (shape2->GetTag() >= 1000) // If object is complex shape made up of many triangles
					{
						for (const auto& triangle : shape2->GetMeshTriangles())
						{
							data = triangle->Intersection(intersectionPoint + rayOrigin, lightRay, secondObjDistance, secondObjIntersectionPoint);

							if (data.intersect)
							{
								// Some weird shadow formula I came up with, makes shadows at 1 light source fully dark, but idk how to make it better
								colors[closestIndex] = colors[closestIndex] * (1.0f - 1.0f / areaLight.GetSampleCount()); // Will become darker and darker the more shadows fall on the pixel.
								break;// Stop the iteration because we already know color value, also in this case break is faster than goto
							}
						}
					}
					else
					{
						// Need to know if we intersect something on lightRay path
						data = shape2->Intersection(intersectionPoint + rayOrigin, lightRay, secondObjDistance, secondObjIntersectionPoint);

						if (data.intersect) // If ray intersects something on its ray path to light, compute color for shadow and add it to finalColor
						{
							// Some weird shadow formula I came up with, makes shadows at 1 light source fully dark, but idk how to make it better
							colors[closestIndex] = colors[closestIndex] * (1.0f - 1.0f / areaLight.GetSampleCount()); // Will become darker and darker the more shadows fall on the pixel
							break;// Stop the iteration because we already know color value, also in this case break is faster than goto
						}
					}
				}
			}
		}
		pointColor = colors[closestIndex]; // Assign color to pixel
	}
}

void Engine::ReflectionRay(AreaLight areaLight, vector<Shape*> shapes, vec3 rayOrigin, vec3 rayDirection, vec3 &pointColor, int depth)
{
	if (depth > reflectionCount) return; // If we reach the max reflection amount, break the function

	IntersectionData data;

	vec3 reflectiveRay, reflectionColor, intersectionPoint, normal;

	float minimumDistance, distance;
	int closestIndex;

	vector<float> distances;
	vector<vec3> colors;
	vector<vec3> intersections;
	vector<IntersectionData> datas;

	// foreach const & is faster than regular for
	for (const auto& shape : shapes)
	{
		// Check if we hit the objects bounding box. Culls the amount of interations which leads to much better performance
		if (shape->GetBoundingBox().CheckForIntersection(rayOrigin, rayDirection))
		{
			// The reason for this two part intersection checking logic is that it is much faster 
			// than doing complex shape logic inside their Intersection function.

			if (shape->GetTag() >= 1000) //In case its a complex object which I would store from 100
			{
				for (const auto& triangle : shape->GetMeshTriangles())
				{
					data = triangle->Intersection(rayOrigin, rayDirection, distance, intersectionPoint);

					if (data.intersect)
					{
						triangle->ComputeColor(data, rayOrigin, rayDirection, areaLight.GetPosition(), areaLight.GetColor(), ambientIntensity, reflectionColor);

						distances.push_back(distance);
						colors.push_back(reflectionColor);
						intersections.push_back(intersectionPoint);
						datas.push_back(data);
					}
				}
			}
			else
			{
				data = shape->Intersection(rayOrigin, rayDirection, distance, intersectionPoint);
				
				if (data.intersect)
				{
					shape->ComputeColor(data, rayOrigin, rayDirection, areaLight.GetPosition(), areaLight.GetColor(), ambientIntensity, reflectionColor);

					distances.push_back(distance);
					colors.push_back(reflectionColor);
					intersections.push_back(intersectionPoint);
					datas.push_back(data);
				}
			}
		}
	}

	if (distances.size() > 0)
	{
		minimumDistance = 1000.0f;
		closestIndex = 0;
		for (int i = 0; i < distances.size(); ++i)
		{
			if (distances[i] < minimumDistance)
			{
				closestIndex = i;
				minimumDistance = distances[i];
			}
		}

		// Apply reflection
		pointColor = pointColor + datas[closestIndex].reflectivity * colors[closestIndex];

		normal = datas[closestIndex].normal;// Get Normal
		intersectionPoint = intersections[closestIndex] + datas[closestIndex].normal * 0.0001f;// Remove self intersecting side effect
		reflectiveRay = normalize(rayDirection - 2 * dot(rayDirection, normal) * normal);// Calculate Reflection

		ReflectionRay(areaLight, shapes, intersectionPoint, reflectiveRay, pointColor, depth + 1);
	}
}

void Engine::PutPixel32_nolock(SDL_Surface*& surface, int x, int y, Uint32 colour)
{
	Uint8* pixel = (Uint8*)surface->pixels;
	pixel += (y * surface->pitch) + (x * sizeof(Uint32));
	*((Uint32*)pixel) = colour;
}

inline Uint32 Engine::ConvertColour(vec3 colour)
{
	int tt;
	Uint8 r, g, b;

	tt = (int)(colour.r * 255);
	if (tt <= 255) r = tt; else r = 255;
	tt = (int)(colour.g * 255);
	if (tt <= 255) g = tt; else g = 255;
	tt = (int)(colour.b * 255);
	if (tt <= 255) b = tt; else b = 255;

	Uint32 rgb;

	//check which order SDL is storing bytes
	rgb = (r << 16) + (g << 8) + b;

	return rgb;
}

inline void Engine::RecalculateTanValue()
{
	tanvalue = tanf(fov * 3.14159265f / 180.0f);
}

int Engine::GetLightSampleCount()
{
	return lightSampleCount;
}

vec3 Engine::GetLightPosition()
{
	return lightPosition;
}

Engine::~Engine()
{
}