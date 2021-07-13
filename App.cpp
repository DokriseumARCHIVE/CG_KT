#define GLM_SWIZZLE
#include <GL/glew.h>
#include "Application.hpp"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "GameObject.hpp"
#include "shader.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <glm/gtx/intersect.hpp>

float camDist = 40.0f;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	const float cameraSpeed = 0.20f;
	switch (key)
	{
	case GLFW_KEY_ESCAPE:
		glfwSetWindowShouldClose(window, GL_TRUE);
		break;
	case GLFW_KEY_Q:
		camDist++;
		break;
	case GLFW_KEY_E:
		if (camDist > 0) {
			camDist--;
		}break;;
	default:
		break;
	}
}

Application::Application(unsigned int width, unsigned int height, const char* title) {
	this->width = width;
	this->height = height;
	this->title = title;


	if (!glfwInit()) {
		fprintf(stderr, "Failed to initialize GLFW\n");
		return;
	}

	this->hwnd = glfwCreateWindow(width, height, title, NULL, NULL);
	glfwMaximizeWindow(hwnd);
}

Application::~Application() {
	glfwDestroyWindow(this->hwnd);
}

void Application::run() {
	if (!hwnd)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	glfwMakeContextCurrent(hwnd);
	glewExperimental = true;

	if (glewInit() != GLEW_OK)
	{
		fprintf(stderr, "Failed to initialize GLEW\n");
		return;
	}
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glfwSetKeyCallback(hwnd, key_callback);
	glClearColor(0.9f, 0.9f, 0.9f, 0.9f);
	programID = LoadShaders("StandardShading.vertexshader", "StandardShading.fragmentshader");

	glUseProgram(programID);
	GameObject go1 = GameObject("sausage.obj");
	std::vector<RenderInformation> renderInformationVector;
	int addingCount = 4;
	renderInformationVector.reserve(addingCount);
	for (int i = 0; i < addingCount; i++) {
		renderInformationVector.push_back(renderHelper(go1));
	}

	float n = 0.1f;
	float f = 100.0f;

	this->projection = glm::perspective(45.0f, 16.0f / 9.0f, n, f);
	this->view = glm::lookAt(glm::vec3(0.0, 0.0f, -camDist), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	while (!glfwWindowShouldClose(this->hwnd)) {
		glfwPollEvents();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::vec3 targetPos = mouseCoordCalculation();

		std::vector<glm::vec3> points;
		points.reserve(renderInformationVector.size() + 1);
		points.push_back(targetPos);

		for (int i = renderInformationVector.size() - 1; i > 0; i--) {
			RenderInformation r = renderInformationVector[i];

			float length = glm::distance(r.highestPoint, r.lowestPoint);
			float remainingLength = length * i;
			float pa = glm::length(points[renderInformationVector.size() - i - 1]);
			float angle = acos((length * length + pa * pa - remainingLength * remainingLength) / (2.0f * length * pa));

			if (std::isnan(angle) || std::isnan(-angle)) {
				if (glm::length(targetPos) > length * renderInformationVector.size()) {
					points.push_back(glm::normalize(points[renderInformationVector.size() - i - 1]) * remainingLength);
					continue;
				}
				else {
					angle = glm::pi<float>();
				}
			}

			glm::vec3 rpa = glm::normalize(glm::vec3(0.0f) - points[renderInformationVector.size() - i - 1]) * length;
			glm::vec3 rpq = points[renderInformationVector.size() - i - 1] + (rpa * glm::angleAxis(i == 1 ? angle : angle / renderInformationVector.size(), glm::vec3(0.0f, 0.0f, 1.0f)));
			points.push_back(rpq);
		}

		points.push_back(glm::vec3(0.0f));

		for (int i = 0; i < renderInformationVector.size(); i++) {
			RenderInformation r = renderInformationVector[i];

			glm::vec3 fromPos = glm::vec4(r.lowestPoint, 1.0f);
			glm::vec3 toPos = glm::inverse(r.renderModel) * glm::vec4(points[i + 1], 1.0f);
			glm::vec3 move = toPos - fromPos;
			r.renderModel = glm::translate(r.renderModel, move);

			glm::vec3 pA = r.renderModel * glm::vec4(r.lowestPoint, 1.0f);
			glm::vec3 pC = r.renderModel * glm::vec4(r.highestPoint, 1.0f);
			glm::vec3 pX = points[i];

			glm::vec3 fromDir = glm::normalize(pC - pA);
			glm::vec3 targetDir = glm::normalize(pX - pA);

			glm::vec3 fromLookAt = glm::normalize(glm::cross(fromDir, glm::vec3(abs(glm::angle(glm::vec3(0.0f, 1.0f, 0.0f), fromDir)) < glm::half_pi<float>() ? 1.0f : -1.0f, 0.0f, 0.0f)));
			glm::vec3 targetLookAt = glm::normalize(glm::cross(targetDir, glm::vec3(abs(glm::angle(glm::vec3(0.0f, 1.0f, 0.0f), targetDir)) < glm::half_pi<float>() ? 1.0f : -1.0f, 0.0f, 0.0f)));

			glm::quat from = glm::quatLookAt(fromLookAt, fromDir);
			glm::quat to = glm::quatLookAt(targetLookAt, targetDir);

			glm::quat diff = (to * glm::inverse(from));
			r.renderModel = glm::mat4_cast(diff) * r.renderModel;
			renderInformationVector[i].renderModel = r.renderModel;
		}

		for (int i = 0; i < renderInformationVector.size(); i++) {
			RenderInformation r = renderInformationVector[i];
			sendMVP(r.renderModel);
			glBindVertexArray(r.renderVertexArray);
			glDrawArrays(GL_TRIANGLES, 0, r.renderVertices.size());
		}
		//2
		glUniform1i(glGetUniformLocation(programID, "myTextureSampler"), 0);
		glm::vec4 lightPos = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -10.0f)) * glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
		glUniform3f(glGetUniformLocation(programID, "LightPosition_worldspace"), lightPos.x, lightPos.y,
			lightPos.z);
		glfwSwapBuffers(this->hwnd);
	}
}

glm::vec3 Application::mouseCoordCalculation() {
	double xpos, ypos;
	glfwGetCursorPos(hwnd, &xpos, &ypos);
	glfwGetWindowSize(hwnd, &width, &height);

	xpos -= (width / static_cast<double>(2));
	ypos -= height / static_cast<double>(2);
	xpos = xpos / (width / 2);
	ypos = ypos / (height / 2);

	float div = 2.92f;

	float viewPosX = (camDist * xpos * 16.0f / 9.0f * tan(45.0f)) / div;
	float viewPosY = (camDist * ypos * tan(45.0f)) / div;
	float viewPosZ = 0.0f;

	return glm::vec3(-viewPosX, -viewPosY, viewPosZ);
}

RenderInformation Application::renderHelper(GameObject go) {
	RenderInformation ri = go.getRenderInformation();
	std::vector<glm::vec3> vertices = ri.renderVertices;
	std::vector<glm::vec2> uvs = ri.renderUvs;
	std::vector<glm::vec3> normals = ri.renderNormals;

	GLuint VertexArrayIDSausage;
	glGenVertexArrays(1, &VertexArrayIDSausage);
	glBindVertexArray(VertexArrayIDSausage);

	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);


	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	GLuint normalbuffer;
	glGenBuffers(1, &normalbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	GLuint uvbuffer;
	glGenBuffers(1, &uvbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), &uvs[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
	RenderInformation rhi = ri;
	rhi.renderVertexArray = VertexArrayIDSausage;
	rhi.renderVertices = vertices;
	rhi.renderModel = ri.renderModel;
	return rhi;
}

void Application::sendMVP(glm::mat4 gameObjectModel) {
	glm::mat4 MVP = projection * view * gameObjectModel;

	glUniformMatrix4fv(glGetUniformLocation(programID, "MVP"), 1, GL_FALSE, &MVP[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(programID, "M"), 1, GL_FALSE, &gameObjectModel[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(programID, "V"), 1, GL_FALSE, &view[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(programID, "P"), 1, GL_FALSE, &projection[0][0]);
}

std::vector<RenderInformation> Application::fullTranslation(std::vector<RenderInformation> ftRenderInformationVector, glm::vec3 translationPosition) {
	std::reverse(ftRenderInformationVector.begin(), ftRenderInformationVector.end());
	RenderInformation ri0 = ftRenderInformationVector.front();
	RenderInformation ri1 = ftRenderInformationVector.at(1);
	for (int i = 0; i < ftRenderInformationVector.size() - 1; i++) {

		ri0 = ftRenderInformationVector.at(i);
		ri1 = ftRenderInformationVector.at(i + 1);

		if (i == 0) {
			ri0.renderModel = glm::translate(ri0.renderModel, translationPosition);
			ri0.renderCoordinates = translationPosition;
		}
		else {
			ri0.renderModel = glm::translate(ri0.renderModel, ri1.renderCoordinates);
		}
		
		float highestPointX = 0.0f, highestPointY = 0.0f, highestPointZ = 0.0f;

		for (int j = 0; j < ri1.renderVertices.size(); j++) {
			if (ri1.renderVertices[j].y > highestPointY) {
				highestPointX = ri1.renderVertices[j].x;
				highestPointY = ri1.renderVertices[j].y;
				highestPointZ = ri1.renderVertices[j].z;
			}
		}
		ri1.renderCoordinates.x = ri0.renderCoordinates.x - highestPointX;
		ri1.renderCoordinates.y = ri0.renderCoordinates.y - highestPointY;
		ri1.renderCoordinates.z = ri0.renderCoordinates.z - highestPointZ;
		ri1.renderModel = glm::translate(ri1.renderModel, ri1.renderCoordinates);
		ftRenderInformationVector.at(i) = ri0;
		ftRenderInformationVector.at(i + 1) = ri1;
	}
	return ftRenderInformationVector;
}