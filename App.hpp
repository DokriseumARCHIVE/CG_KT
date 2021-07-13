#pragma once
#ifndef _APPLICATION_H
#define _APPLICATION_H

#include "GLFW/glfw3.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "GameObject.hpp"
#include "RenderInformation.hpp"

//1
class Application {

public:
	Application(unsigned int width = 500, unsigned int height = 500, const char* title = "Otto");
	~Application();

	void run();
	glm::vec3 mouseCoordCalculation();
	RenderInformation renderHelper(GameObject go);
	void sendMVP(glm::mat4 gameObjectModel);
	std::vector<RenderInformation> fullTranslation(std::vector<RenderInformation> ftRenderInformationVector, glm::vec3 translationPosition);

private:
	GLFWwindow* hwnd;
	int width, height;
	const char* title;
	unsigned int programID;
	glm::mat4 projection;
	glm::mat4 view;

};

#endif 