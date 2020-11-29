#pragma once
#ifndef LIGHT
#define LIGHT

#include <glm/glm.hpp>
#include "Program.h"
#include "Shape.h"

class Light {

public:
	//glUniform3f(prog->getUniform("lightPos"), 1.0f, 1.0f, 1.0f);
	std::shared_ptr<Program> prog;
	glm::vec3 LightPos;
	float intensity;
	std::string uniformposName;
	std::string uniformintName;
	Light();
	Light(std::shared_ptr<Program> Program);

	static void createLightForProject(std::shared_ptr<Program>& Program, Light& L1, Light& L2);

	void SetShaderLight();
};

#endif