#pragma once
#ifndef _GAMEOBJECT_H
#define _GAMEOBJECT_H

#include <stdio.h>
#include <stdlib.h>
#include "GLFW/glfw3.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include "ObjectLoader.hpp"
#include "RenderInformation.hpp"

class GameObject {
private:
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec3> location;
	unsigned int rotation;
	unsigned int programID;
	unsigned int textureID;
	glm::mat4 gameObjectModel;
	glm::vec3 highestPoint;
	glm::vec3 lowestPoint;


public:
	GameObject(const char* path);
	~GameObject();

	RenderInformation getRenderInformation();
};
#endif