#pragma once
#ifndef _MESH_H
#define _MESH_H

#include <stdio.h>
#include <stdlib.h>
#include "GLFW/glfw3.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
bool loadOBJ(const char* path,
	std::vector<glm::vec3>& out_vertices,
	std::vector<glm::vec2>& out_uvs,
	std::vector<glm::vec3>& out_normals);

#endif
