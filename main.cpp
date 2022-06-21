#include "Engine.h"
#include "Sphere.h"
#include "Plane.h"
#include "Triangle.h"
#include "ComplexShape.h"

vector<Shape*> InitShapes()
{
	vector<Shape*> shapes;
	int tag = 0;

	// Create objects and give them properties.
	Shape* redSphere = new Sphere(4.0f, vec3(0.0f, 0.0f, -20.0f), vec3(1.0f, 0.32f, 0.36f), ++tag);
	redSphere->SetSpecularColor(vec3(0.6f, 0.2f, 0.3f));
	redSphere->SetShininess(10);
	shapes.push_back(redSphere);

	//Shape* yellowSphere = new Sphere(1.0f, vec3(4.0f, -2.5f, -15.0f), vec3(0.9f, 0.76f, 0.46f), ++tag);
	//yellowSphere->SetReflectivity(0.5f);
	//shapes.push_back(yellowSphere);

	Shape* blueSphere = new Sphere(3.2f, vec3(6.3f, -0.4f, -15.0f), vec3(0.65f, 0.77f, 0.97f), ++tag);
	shapes.push_back(blueSphere);

	Shape* greySphere = new Sphere(3.0f, vec3(-7.5f, 0.0f, -24.0f), vec3(0.9f, 0.9f, 0.9f), ++tag);
	greySphere->SetSpecularColor(vec3(0.0f, 0.1f, 0.1f));
	//greySphere->SetShininess(1000);
	shapes.push_back(greySphere);

	Shape* greyPlane = new Plane(vec3(0.0f, -4.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f), vec3(0.9f, 0.9f, 0.9f), ++tag);
	greyPlane->SetReflectivity(0.05f);
	shapes.push_back(greyPlane);

	Shape* oliveTriangle = new Triangle(vec3(0.0f, -1.8f, -8.0f), vec3(-1.9f, -3.8f, -8.0f), vec3(1.6f, -3.3f, -8.0f), // Vertice position
		vec3(0.0f, 2.0f, 1.0f), vec3(-2.0f, -0.8f, 1.0f), vec3(2.0f, -0.8f, 1.0f), // Normals
		vec3(0.5f, 0.5f, 0.0f), ++tag); // Color
	shapes.push_back(oliveTriangle);

	tag = 1000; // Complex objects from here

	//Shape* cube = new ComplexShape("models/Cube.obj", vec3(-10.0f, -4.0f, -30.0f), vec3(1.0f, 1.0f, 1.0f), ++tag);
	//shapes.push_back(cube);

	//Shape* cylinder = new ComplexShape("models/Cylinder.obj", vec3(7.0f, -4.0f, -20.0f), vec3(0.1f, 0.2f, 0.8f), ++tag);
	//shapes.push_back(cylinder);

	Shape* helmet = new ComplexShape("models/Helmet.obj", vec3(-5.0f, -4.0f, -13.0f), vec3(0.5f, 0.5f, 0.8f), ++tag);
	helmet->SetReflectivity(0.3f);
	shapes.push_back(helmet);

	return shapes;
}

int main(int argc, char* args[])
{
	// Engine settings can be adjusted in Engine.h
	Engine engine;

	// Light
	AreaLight areaLight(engine.GetLightSampleCount(), engine.GetLightPosition(), vec3(9.0f, 0.1f, 9.0f), vec3(1.0f, 1.0f, 1.0f));

	// All objects array
	vector<Shape*> shapes = InitShapes();

	engine.StartUpdate(areaLight, shapes);
	engine.CloseEngine(shapes);

	return 0;
}