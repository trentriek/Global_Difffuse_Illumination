#pragma  once
#ifndef MATERIAL
#define MATERIAL

#include <glm/glm.hpp>
#include "Program.h"
#include "Shape.h"

class Material {

public:
	std::shared_ptr<Program> prog;
	glm::vec3 ka; //ambiance
	glm::vec3 kd; //diffuse
	glm::vec3 ks; //specularity
	float s; //scale of light
	Material();
	Material(std::shared_ptr<Program>& Program);
	Material(std::shared_ptr<Program>& Program, glm::vec3 KA, glm::vec3 KD, glm::vec3 KS, float S);
	void SetShaderToMat(std::shared_ptr<Program>& Program);

	static void CreateMaterialsForProject(std::shared_ptr<Program>& Program, Material& M1, Material& M2, Material& M3, Material& Toon);

};


#endif