#include "Light.h"

using namespace std;
Light::Light() {
	LightPos.x = 1.0f; LightPos.y = 1.0f; LightPos.z = 1.0f;
	float intensity = 1.0f;
	uniformposName = "";
	uniformintName = "";
}
Light::Light(std::shared_ptr<Program> Program) {
	prog = Program;
}

void Light::SetShaderLight(std::shared_ptr<Program>& Program) {
	glUniform3f(Program->getUniform(uniformposName), LightPos.x, LightPos.y, LightPos.z);
	glUniform1f(Program->getUniform(uniformintName), intensity);
}

void Light::createLightForProject(std::shared_ptr<Program>& Program, Light& L1, Light& L2) {

	L1.prog = Program;
	L2.prog = Program;

	L1.LightPos.x = 1.0f; L1.LightPos.y = 1.0; L1.LightPos.z = 1.0;
	L1.intensity = 0.8f; L1.uniformposName = "lightPos"; L1.uniformintName = "lightInt";
	L2.LightPos.x = -1.0f; L2.LightPos.y = 1.0; L2.LightPos.z = 1.0;
	L2.intensity = 0.2f; L2.uniformposName = "lightPos2"; L2.uniformintName = "lightInt2";
}
