#include <GL/glew.h>
#include "GameObject.hpp"
#include "ObjectLoader.hpp"

GameObject::GameObject(const char* path)
{
	bool res = loadOBJ(path, vertices, uvs, normals);
	gameObjectModel = glm::mat4(1.0f);
	float highestPointX = 0.0f, highestPointY = 0.0f, highestPointZ = 0.0f;
	float lowestPointX = 0.0f, lowestPointY = 0.0f, lowestPointZ = 0.0f;

	for (int i = 0; i < vertices.size(); i++) {
		if (vertices[i].y < lowestPointY) {
			lowestPointX = vertices[i].x;
			lowestPointY = vertices[i].y;
			lowestPointZ = vertices[i].z;
		}
	}

	for (int i = 0; i < vertices.size(); i++) {
		if (vertices[i].y > highestPointY) {
			highestPointX = vertices[i].x;
			highestPointY = vertices[i].y;
			highestPointZ = vertices[i].z;
		}
	}

	printf("lowest x: %f\n", lowestPointX);
	printf("lowest y: %f\n", lowestPointY);
	printf("lowest z: %f\n", lowestPointZ);
	printf("highest x: %f\n", highestPointX);
	printf("highest y: %f\n", highestPointY);
	printf("highest z: %f\n", highestPointZ);

	this->highestPoint = glm::vec3(highestPointX, highestPointY, highestPointZ);
	this->lowestPoint = glm::vec3(lowestPointX, lowestPointY, lowestPointZ);
}

GameObject::~GameObject()
{
}

RenderInformation GameObject::getRenderInformation()
{
	RenderInformation renderInformation = RenderInformation();
	renderInformation.renderModel = this->gameObjectModel;
	renderInformation.renderVertices = this->vertices;
	renderInformation.renderUvs = this->uvs;
	renderInformation.renderNormals = this->normals;
	renderInformation.highestPoint = this->highestPoint;
	renderInformation.lowestPoint = this->lowestPoint;

	return renderInformation;
}