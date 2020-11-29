#include "Material.h"

Material::Material() {
	//prog = Program;
	ka.x = 0.0f; ka.y = 0.0f; ka.z = 0.0f;
	kd.x = 0.0f; kd.y = 0.0f; kd.z = 0.0f;
	ks.x = 0.0f; ks.y = 0.0f; ks.z = 0.0f;
	s = 0.0f;
}
Material::Material(std::shared_ptr<Program>& Program){
	prog = Program;
	ka.x = 0.0f; ka.y = 0.0f; ka.z = 0.0f;
	kd.x = 0.0f; kd.y = 0.0f; kd.z = 0.0f;
	ks.x = 0.0f; ks.y = 0.0f; ks.z = 0.0f;
	s = 0.0f;
}
Material::Material(std::shared_ptr<Program>& Program, glm::vec3 KA, glm::vec3 KD, glm::vec3 KS, float S) {
	prog = Program;
	ka = KA;
	kd = KD;
	ks = KS;
	s = S;
}
void Material::SetShaderToMat() {
	glUniform3f(prog->getUniform("ka"), ka.x, ka.y, ka.z);
	glUniform3f(prog->getUniform("kd"), kd.x, kd.y, kd.z);
	glUniform3f(prog->getUniform("ks"), ks.x, ks.y, ks.z);
	glUniform1f(prog->getUniform("s"), s);
}

void Material::CreateMaterialsForProject(std::shared_ptr<Program>& Program, Material& M1, Material& M2, Material& M3, Material& Toon) {
	M1.prog = Program;
	M1.ka.x = 0.2f; M1.ka.y = 0.2f; M1.ka.z = 0.2f;
	M1.kd.x = 0.8f; M1.kd.y = 0.7f; M1.kd.z = 0.7f;
	M1.ks.x = 1.0f; M1.ks.y = 0.9f; M1.ks.z = 0.8f;
	M1.s = 200.0f; //Move this

	//now need m2 & m3
	M2.prog = Program;
	M2.ka.x = 0.0f; M2.ka.y = 0.0f; M2.ka.z = 0.1f;
	M2.kd.x = 0.7f; M2.kd.y = 0.7f; M2.kd.z = 0.7f;
	M2.ks.x = 0.1f; M2.ks.y = 0.1f; M2.ks.z = 0.1f;
	M2.s = 200.0f;

	M3.prog = Program;
	M3.ka.x = 0.1f; M3.ka.y = 0.1f; M3.ka.z = 0.5f;
	M3.kd.x = 0.1f; M3.kd.y = 0.1; M3.kd.z = 0.9f;
	M3.ks.x = 0.0f; M3.ks.y = 1.0f; M3.ks.z = 0.4f;
	M3.s = 200.0f;

	Toon.prog = Program;
}
/*
glUniform3f(prog->getUniform("ka"), 0.2f, 0.2f, 0.2f);
glUniform3f(prog->getUniform("kd"), 0.8f, 0.7, 0.7f);
glUniform3f(prog->getUniform("ks"), 1.0f, 0.9f, 0.8f);
glUniform1f(prog->getUniform("s"), 200.0f);
*/